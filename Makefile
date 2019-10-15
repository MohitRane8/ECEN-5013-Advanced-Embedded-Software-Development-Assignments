CC = $(CROSS_COMPILE)gcc
CFLAGS= -g -Wall -Werror

all: writer aesdsocket

writer: writer.c
	$(CC) $(CFLAGS) -o writer writer.c

aesdsocket: server/aesdsocket.c
	$(CC) $(CFLAGS) -o server/aesdsocket server/aesdsocket.c -lpthread

clean:
	-rm -f *.o *.d server/*.o server/*.d
	-rm -f writer server/aesdsocket
