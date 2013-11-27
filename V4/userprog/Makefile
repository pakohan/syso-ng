
all: generic

generic: userprog.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(USERPROG_STATIC) $^ -o userprog

clean:
	rm -f *.o userprog
