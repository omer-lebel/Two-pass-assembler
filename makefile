.PHONY: clean

CC=gcc
CFLAGS= -Wall -ansi -pedantic

test: preAssembler
	./test.sh

preAssembler: test.o preAssembler.o input.o linkedList.o
	$(CC) $(CFLAGS)  test.o preAssembler.o input.o linkedList.o -o preAssembler

test.o: test.c
	$(CC) $(CFLAGS) -c test.c

preAssembler.o: preAssembler.c preAssembler.h
	$(CC) $(CFLAGS) -c preAssembler.c

input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c

linkedList.o: linkedList.c linkedList.h
	$(CC) $(CFLAGS) -c linkedList.c

clean:
	rm -f *.o preAssembler