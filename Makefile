ifeq ($(CC),)
	CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(CCFLAGS),)
	CFLAGS = -g -Wall -Werror
endif

ifeq ($(LDFLAGS),)
	LDFLAGS = -lpthread -lrt
endif

all: writer aesdsocket

writer: writer.c
	$(CC) $(CFLAGS) -o writer writer.c

aesdsocket: server/aesdsocket.c
	$(CC) $(CFLAGS) -o server/aesdsocket server/aesdsocket.c $(LDFLAGS)

clean:
	-rm -f *.o *.d server/*.o server/*.d
	-rm -f writer server/aesdsocket