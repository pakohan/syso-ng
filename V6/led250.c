#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

#define DEVICE "/dev/mygpio"

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
    
    new_action.sa_handler = signal_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(SIGINT, &new_action, NULL);

	device = open(DEVICE, O_RDWR);
	if (device < 0)
	{
		fprintf(stderr, "could not open " DEVICE "\n");
	}	
	
    if (pthread_create(&blinker, NULL, &blink_worker, (void*) device) != 0)
    {
        fprintf(stderr, "could not create thread\n");
        return 1;
    }
    
    int val;
    char value[2];

    while(1)
    {
        read(device, value, 2);
        
        int val2 = atoi(value);
                
        if(val2 == 1)
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
    
	close(device);
      
    return 0;
}

void *blink_worker(void *arg)
{
	struct timespec times;
	times.tv_sec = 0;
	times.tv_nsec = 4000000;
	
	int led = (int) arg;
	
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

	return NULL;
}

void signal_handler(int value)
{
	our_signal = value;
	printf("Signal caught\n");
	int led = open(DEVICE, O_RDWR);
	if (led < 0)
	{
		fprintf(stderr, "could not open " DEVICE "\n");
		_exit(value);
	}
	turn_off(led);

	if( close(led) == EOF )
    {
		fprintf(stderr, "could not close " DEVICE "\n");
    }
		
    _exit(value);
}

void turn_on(int fpointer)
{
	write(fpointer, "1", 1);
}

void turn_off(int fpointer)
{
	write(fpointer, "0", 1);
}
