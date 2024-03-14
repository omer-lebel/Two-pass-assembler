.PHONY: clean

CC=gcc
CFLAGS = -Wall -Wextra -Wvla -ansi -pedantic
#HUJI = -Wextra -Wall -Wvla -std=c99

OBJECTS =  vector.o linkedList.o setting.o machineWord.o text.o \
		symbolTable.o ast.o externTable.o entryTable.o memoryImg.o \
		preAssembler.o fsm.o firstPass.o secondPass.o

assembler: main.o
	$(CC) $(CFLAGS) main.o $(OBJECTS) -o assembler


# Define rules for building object files from utils
%.o: utils/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Define rules for building object files from fileStructures
%.o: fileStructures/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Define rules for building object files from source files
%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

# Dependencies
main.o: $(OBJECTS)
secondPass.o: memoryImg.o externTable.o entryTable.o #text.o setting.o linkedList.o vector.o symbolTable.o ast.o
firstPass.o: ast.o fsm.o entryTable.o # text.o setting.o linkedList.o vector.o symbolTable.o memoryImg.o
fsm.o: text.o symbolTable.o memoryImg.o #setting linkedList.o
preAssembler.o: text.o linkedList.o #setting.o
memoryImg.o: machineWord.o symbolTable.o ast.o #setting.o text.o vector.o machineWord.o symbolTable.o vector.o ast.o
externTable.o: setting.o #vector.o
entryTable.o: text.o #setting.o vector.o
ast.o: text.o #setting.o linkedList.o
symbolTable.o: setting.o #linkedList.o
text.o: setting.o
machineWord.o: setting.o
setting.o: linkedList.o vector.o

# Clean rule
clean:
	rm -f *.o assembler