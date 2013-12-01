#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <errno.h>

/*TODO TEST device with open, close, read, write*/

int  main(int argc, char **argv)
{
    int i, j = atoi(argv[2]);
    size_t x;
    FILE *files[j];

    char buf[100];

    if (argc < 3) {
        return -1;
    }

    for (i = 0; i < j; i++) {
        files[i] = fopen(argv[1], "r+w");
        if (files[i] != NULL) {
            printf("Opening success\n");

            x = fread(buf, sizeof(buf), 1, files[i]);
            printf("red %d blocks: %s\n", x, buf);
            printf("wrote %d bytes\n", fwrite("hello driver", sizeof("hello driver"), sizeof("hello driver"), files[i]));

            /*fclose(files[i]);*/
        } else {
            printf("Opening failed: %d\n", errno);
        }
    }
}

