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

void turn_on(int fpointer);
void turn_off(int fpointer);

#define NUMBER 10

double timevaldiff(struct timeval *starttime, struct timeval *finishtime)
{
  double usec;
  usec=(finishtime->tv_sec-starttime->tv_sec)*1000000.0;
  usec+=(finishtime->tv_usec-starttime->tv_usec);
  return usec;
}

int  main()
{
    pthread_t blinker;
    struct sigaction new_action;
    int device, i;
	struct timeval start, end;

	device = open(DEVICE, O_RDWR);
	if (device < 0)
	{
		fprintf(stderr, "could not open " DEVICE "\n");
	}	
	
	gettimeofday(&start, NULL);
	for (i = 0; i < NUMBER; i++)
	{
		turn_on(device);
		turn_off(device);
    }
    gettimeofday(&end, NULL);
    	
	printf("%lf usec\n", timevaldiff(&start, &end) / NUMBER);

	close(device);
      
    return 0;
}


void turn_on(int fpointer)
{
	write(fpointer, "1", 1);
}

void turn_off(int fpointer)
{
	write(fpointer, "0", 1);
}
