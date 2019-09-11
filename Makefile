CC=gcc
CFLAGS=-I.
DEPS =
OBJ = writer.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

writer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	-rm -f *.o *.d
	-rm -f writer