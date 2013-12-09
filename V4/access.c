#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <errno.h>

/*TODO TEST device with open, close, read, write*/

int test(char* path, char* mode);


int  main()
{
    int retVal;
    test("/dev/mod_openclose", "r+w");
    test("/dev/mod_hello", "r+w");
    test("/dev/mod_zero0", "r+w");
    test("/dev/mod_null", "r+w");


    return 0;
}


int test(char* path, char* mode)
{
    int retVal = 0;
    FILE *file;
    char buf[100];

    printf("Started test for device %s with mode %s.\n", path, mode);

    file = fopen(path, mode);
    if( file == NULL )
    {      
        printf("Open failed - %s\n", strerror(errno));  
        return errno;
    }
    printf("Opened\n");
    
    retVal = 0;
    retVal = fread(buf, sizeof(buf), 1, file);
    printf("Read %d blocks: %s\n", retVal, buf);

    retVal = 0;
    retVal = fwrite("hello driver", sizeof("hello driver"), sizeof("hello driver"), file);
    printf("Wrote %d bytes\n", retVal);

    retVal = 0;
    retVal = fclose(file);
    if( retVal == EOF )
    {   
        printf("Close failed - %s\n", strerror(errno));
        return errno;
    }

    printf("Closed\n");
    printf("Finished test for device %s with mode %s.\n\n", path, mode);
    return 0;
}
