CC = $(CROSS_COMPILE)gcc
CFLAGS= -g -Wall -Werror
OBJ = writer.o

all: writer

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

writer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	-rm -f *.o *.d
	-rm -f writer