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



static int counter = 0;

void turn_on(int fpointer);
void turn_off(int fpointer);

int  main()
{
    int led;
    int button;
    int val;
    char value[1];

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
	
	int direction18 = open(LED "direction", O_WRONLY);
	if (export < 0)
	{
		fprintf(stderr, "could not open '%s'\n", LED "direction");
	}
	
	int direction25 = open(BUTTON "direction", O_WRONLY);
	if (export < 0)
	{
		fprintf(stderr, "could not open '%s'\n", BUTTON "direction");
	}
	
	write(direction18, "out", 3);
	write(direction25, "in", 2);
	
	close(direction18);
	close(direction25);
   
    
    led = open(LED "value" , O_WRONLY);
    if (led < 0)
	{
		fprintf(stderr, "could not open '%s'\n", LED);
	}
	
    turn_off(led);
    
    printf("Start:\n");
    while(1)
    {
        val = open(BUTTON "value", O_RDONLY);
        if (led < 0)
        {
	        fprintf(stderr, "could not open '%s'\n", BUTTON "value");
        }
        read(val, value, 1);
        if(atoi(value) == 0)
            counter++;
            
        printf("Button pressed: %d\n", counter);
        close(val);
        usleep(100000);
    }
    
    return 0;
}




void turn_on(int fpointer)
{
	write(fpointer, "0", 1);
}

void turn_off(int fpointer)
{
	write(fpointer, "1", 1);
}
