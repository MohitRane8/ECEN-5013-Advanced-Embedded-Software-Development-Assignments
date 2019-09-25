CC=gcc
CROSS_COMPILE=
CFLAGS= -g -Wall -Werror
OBJ = writer.o

all: writer

%.o: %.c
	$(CROSS_COMPILE)$(CC) -c -o $@ $< $(CFLAGS)

writer: $(OBJ)
	$(CROSS_COMPILE)$(CC) -o $@ $^ $(CFLAGS)

clean:
	-rm -f *.o *.d
	-rm -f writer