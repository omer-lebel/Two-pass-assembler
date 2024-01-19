.PHONY: clean all testPre testFirst

CC=gcc
CFLAGS = -Wall -Wextra -Wvla -ansi -pedantic
HUJI = -Wextra -Wall -Wvla -std=c99


all: testFirst

################## pre assembler test ##################

testPre: preAssembler
	./testing/testPre/testPre.sh

preAssembler: testPre.o preAssembler.o input.o linkedList.o
	$(CC) $(CFLAGS) testPre.o preAssembler.o input.o linkedList.o -o preAssembler

testPre.o: ./testing/testPre/testPre.c
	$(CC) $(CFLAGS) -c ./testing/testPre/testPre.c -o testPre.o



################## first pass test ##################
testFirst: firstPass
	./testing/testFirst/testFirst.sh

firstPass: testFirst.o firstPass.o preAssembler.o input.o linkedList.o
	$(CC) $(CFLAGS) testFirst.o firstPass.o preAssembler.o input.o linkedList.o -o firstPass

testFirst.o: testing/testFirst/testFirst.c
	$(CC) $(CFLAGS) -c testing/testFirst/testFirst.c -o testFirst.o




################## o files #######################
firstPass.o: firstPass.c firstPass.h
	$(CC) $(CFLAGS) -c firstPass.c

preAssembler.o: preAssembler.c preAssembler.h
	$(CC) $(CFLAGS) -c preAssembler.c

input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c

linkedList.o: linkedList.c linkedList.h
	$(CC) $(CFLAGS) -c linkedList.c



clean:
	rm -f *.o preAssembler firstPass