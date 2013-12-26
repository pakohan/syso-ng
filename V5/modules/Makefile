TP_SOURCES=buf.c hello2.c tasklet.c timer.c kthread.c wq.c lock.c open_once.c buf_threaded.c
TP_OBJECTS=$(TP_SOURCES:.c=.o)

ifneq ($(KERNELRELEASE),)
obj-m := $(TP_OBJECTS)

else
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
endif

clean:
	rm -rf *.o *.ko *.mod.c *.order *.symvers .*cmd .tmp_versions
