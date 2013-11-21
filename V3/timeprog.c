#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>

int  main(void)
{
    struct sysinfo *info;
    int code;
    info = (struct sysinfo*) malloc(sizeof(struct sysinfo));
    if (info == NULL) {
        printf("malloc failed");
    } else {
		
		
		while(1)
		{
			code = sysinfo(info);
			if (code != 0) {
				printf("sysinfo failed");
			} else {
				printf("Hello User world\nUptime: %ld\n", info->uptime);
			}
			sleep(5);
		}
    }

    return 0;
}

