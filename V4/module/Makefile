TP_SOURCES=$(wildcard *.c)
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
	rm -rf *.o *.ko *.mod.c *.order *.symvers .*
