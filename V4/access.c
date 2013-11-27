#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>

TODO TEST device with open, close, read, write

int  main(int arc, char **argv)
{
    file_t *files = (file_t*) malloc(atoi(argv[2])*sizeof(file_t*))

    if (argc < 3) {
        return -1;
    }

    for (int i = 0; i < atoi(argv[2]); i++) {
        int err = open(argv[1], file[i]);
        if (!err) {
            printf("Opening success\n");

            printf(read(file[i], 10));

            sprintf(write(file[i], "hello driver"));

            close(file[i])
        } else {
            printf("Opening failed\n");
        }
    }
}

