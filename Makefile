ifeq ($(CC),)
	CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(CCFLAGS),)
	CFLAGS = -g -Wall -Werror
endif

ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lgcc_s -lrt
endif

ifeq ($(OPTFLAGS),)
	OPTFLAGS = -O0
endif

all: writer aesdsocket

writer: writer.c
	$(CC) $(CCFLAGS) -o writer writer.c

aesdsocket: server/aesdsocket.c
	$(CC) $(CCFLAGS) $(OPTFLAGS) -o server/aesdsocket server/aesdsocket.c $(LDFLAGS)

clean:
	-rm -f *.o *.d server/*.o server/*.d
	-rm -f writer server/aesdsocket