.PHONY: clean all testPre testFirst

CC=gcc
CFLAGS = -Wall -Wextra -Wvla -ansi -pedantic
HUJI = -Wextra -Wall -Wvla -std=c99


all: testFirst

################## pre assembler test ##################

testPre: preAssembler
	./testing/testPre/testPre.sh

preAssembler: testPre.o preAssembler.o text.o linkedList.o
	$(CC) $(CFLAGS) testPre.o preAssembler.o text.o linkedList.o -o preAssembler

testPre.o: ./testing/testPre/testPre.c
	$(CC) $(CFLAGS) -c ./testing/testPre/testPre.c -o testPre.o



################## first pass test ##################
testFirst: firstPass
	./testing/testFirst/testFirst.sh

firstPass: testFirst.o firstPass.o fsm.o preAssembler.o text.o linkedList.o symbolTable.o memoryImg.o setting.o
	$(CC) $(CFLAGS) testFirst.o firstPass.o fsm.o preAssembler.o text.o linkedList.o symbolTable.o memoryImg.o setting.o -o firstPass

testFirst.o: testing/testFirst/testFirst.c
	$(CC) $(CFLAGS) -c testing/testFirst/testFirst.c -o testFirst.o


################## generic #######################
setting.o: setting.c setting.h
	$(CC) $(CFLAGS) -c setting.c

################## utils #######################

linkedList.o: utils/linkedList.c utils/linkedList.h setting.h
	$(CC) $(CFLAGS) -c utils/linkedList.c

text.o: utils/text.c utils/text.h setting.h
	$(CC) $(CFLAGS) -c utils/text.c

fsm.o: fsm.c fsm.h fileStructures/symbolTable.c fileStructures/memoryImg.h utils/text.h setting.h
	$(CC) $(HUJI) -c fsm.c

################## file structures #######################

symbolTable.o: fileStructures/symbolTable.c fileStructures/symbolTable.h utils/linkedList.h setting.h
	$(CC) $(CFLAGS) -c fileStructures/symbolTable.c

memoryImg.o: fileStructures/memoryImg.c fileStructures/memoryImg.h utils/text.h setting.h
	$(CC) $(CFLAGS) -c fileStructures/memoryImg.c

################## program running #######################

preAssembler.o: preAssembler.c preAssembler.h utils/text.h utils/linkedList.h
	$(CC) $(CFLAGS) -c preAssembler.c

firstPass.o: firstPass.c firstPass.h fileStructures/symbolTable.h fileStructures/memoryImg.h
	$(CC) $(CFLAGS) -c firstPass.c








clean:
	rm -f *.o preAssembler firstPass