CC=gcc
#CROSS_COMPILE=arm-unknown-linux-gnueabi-
#CFLAGS=-I.
CROSS_COMPILE=
CFLAGS=
DEPS =
OBJ = writer.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	#$(CC) -c -o $@ $< CROSS_COMPILE=$(CROSS) $(CFLAGS)

writer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	-rm -f *.o *.d
	-rm -f writer