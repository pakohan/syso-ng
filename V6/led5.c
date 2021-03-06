#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

#define EXPORT "/sys/class/gpio/export"
#define UNEXPORT "/sys/class/gpio/unexport"

#define LED "/sys/class/gpio/gpio18/"
#define BUTTON "/sys/class/gpio/gpio25/"

static volatile int our_signal = 0;

void *blink_worker(void *arg);
void signal_handler(int value);
void turn_on(int fpointer);
void turn_off(int fpointer);

static int button_state = 0;

int  main()
{
    pthread_t blinker;
    struct sigaction new_action;
    int device;

    int button;
    int led;
    
    int export = open(EXPORT, O_WRONLY);
	if (export < 0)
	{
		fprintf(stderr, "could not open '%s'\n", EXPORT);
	}
	write(export, "18", 2);
	write(export, "25", 2);
    if( close(export) == EOF )
    {
		fprintf(stderr, "could not close '%s'\n", EXPORT);
        return errno;
    }

    new_action.sa_handler = signal_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(SIGINT, &new_action, NULL);


	
	int direction25 = open(BUTTON "direction", O_WRONLY);
	if (export < 0)
	{
		fprintf(stderr, "could not open '%s'\n", BUTTON "direction");
	}

	write(direction25, "in", 2);
	close(direction25);
	
	
    if (pthread_create(&blinker, NULL, &blink_worker, NULL) != 0)
    {
        fprintf(stderr, "could not create thread\n");
        return 1;
    }
    


    int val;
    char value[2];

    while(1)
    {
    	device = open(BUTTON "value", O_RDONLY);
		if (device < 0)
		{
			fprintf(stderr, "could not open '%s'\n", BUTTON "value");
		}
		read(device, value, 2);
		close(device);
        
        val = atoi(value);
                
        if(val == 0)
        {
			if (button_state == 0)
			{
				button_state = 1;
				printf("button down\n");
			}
        }
        else
        {
			if (button_state == 1)
			{
				button_state = 0;
				our_signal = !our_signal;
				printf("button up\n");
			}
		}

        sched_yield();
        usleep(20000);
    }
    
    
    pthread_join(blinker, NULL);
    
    

      
    return 0;
}

void *blink_worker(void *arg)
{
	struct timespec times;
	times.tv_sec = 0;
	times.tv_nsec = 100000000;
	
	int led = open(LED "direction", O_WRONLY);
	if (led < 0)
	{
		fprintf(stderr, "could not open '%s'\n", LED "direction");
	}
	write(led, "out", 3);
	if( close(led) == EOF )
    {
		fprintf(stderr, "could not close '%s'\n", LED "direction");
        return NULL;
    }

	led = open(LED "value", O_WRONLY);
	if (led < 0)
	{
		fprintf(stderr, "could not open '%s'\n", LED "direction");
	}

    while(1)
    {
	    while (!our_signal)
	    {
		    clock_nanosleep(CLOCK_REALTIME, 0, &times, NULL);
		    turn_on(led);
		    clock_nanosleep(CLOCK_REALTIME, 0, &times, NULL);
		    turn_off(led);
	    }
	    turn_off(led);
	}
	
	if( close(led) == EOF )
    {
		fprintf(stderr, "could not close '%s'\n", LED);
        return NULL;
    }

	return NULL;
}

void signal_handler(int value)
{
	our_signal = value;
	printf("Signal caught\n");
	int led = open(LED "value", O_WRONLY);
	if (led < 0)
	{
		fprintf(stderr, "could not open '%s'\n", LED "direction");
	}
	write(led, "1", 1);

	if( close(led) == EOF )
    {
		fprintf(stderr, "could not close '%s'\n", LED);
    }
	
	int unexport = open(UNEXPORT, O_WRONLY);
	if (unexport < 0)
	{
		fprintf(stderr, "could not open '%s'\n", EXPORT);
	}
	
	write(unexport, "18", 2);
	write(unexport, "25", 2);
	
	close(unexport);
	
    _exit(value);
}

void turn_on(int fpointer)
{
	write(fpointer, "0", 1);
}

void turn_off(int fpointer)
{
	write(fpointer, "1", 1);
}
