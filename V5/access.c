#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

/*TODO TEST device with open, close, read, write*/

int test(char* path, int mode);


int  main()
{
    int retVal;
    test("/dev/mod_buf0", O_RDONLY);


    return 0;
}


int test(char* path, int mode)
{
    int retVal = 0;
    int file;
    char buf[6];

    printf("Started test for device %s\n", path);

    file = open(path, mode);
    if( file < 0 )
    {
        printf("Open failed - %s\n", strerror(errno));
        return errno;
    }
    printf("Opened\n");

    retVal = 0;
    retVal = read(file, buf, 5);

    buf[5] = '\0';

    printf("Read %d blocks: %s\n", retVal, buf);

    retVal = 0;
    //retVal = fwrite("hello driver", sizeof("hello driver"), sizeof("hello driver"), file);
    //printf("Wrote %d bytes\n", retVal);

    retVal = 0;
    retVal = close(file);
    if( retVal == EOF )
    {
        printf("Close failed - %s\n", strerror(errno));
        return errno;
    }

    printf("Closed\n");
    printf("Finished test for device %s with mode %s.\n\n", path, mode);
    return 0;
}
