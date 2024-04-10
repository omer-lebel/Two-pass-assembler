.PHONY: clean

CC=gcc
CFLAGS = -Wall -Wextra -ansi -pedantic -Wunused-parameter
#HUJI = -Wextra -Wall -Wvla -std=c99

BASE_UTILS = setting.o text.o errors.o

FILE_STRUCTURES = codeSeg.o dataSeg.o symbolTable.o entryLines.o fileStructures.o

OBJECTS =  setting.o text.o errors.o \
           linkedList.o vector.o machineWord.o \
           symbolTable.o dataSeg.o codeSeg.o entryLines.o fileStructures.o externUsages.o \
           preAssembler.o \
           analysis.o fsm.o firstAnalysis.o firstPass.o \
           secondPass.o


assembler: assembler.o
	$(CC) $(CFLAGS) *.o -o assembler


############################  building

# building object files from utils
%.o: utils/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# building object files from fileStructures
%.o: fileStructures/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# building object files from source
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

############################  Dependencies
assembler.o: 		$(OBJECTS)
secondPass.o: 		$(BASE_UTILS) $(FILE_STRUCTURES) externUsages.o
firstPass.o: 		$(BASE_UTILS) $(FILE_STRUCTURES) firstAnalysis.o
firstAnalysis.o: 	$(BASE_UTILS) codeSeg.o dataSeg.o symbolTable.o analysis.o fsm.o
fsm.o: 				$(BASE_UTILS) codeSeg.o dataSeg.o symbolTable.o analysis.o
analysis.o: 		$(BASE_UTILS) codeSeg.o dataSeg.o symbolTable.o entryLines.o fileStructures.o
preAssembler.o: 	$(BASE_UTILS) linkedList.o fileStructures.o

# dependencies of files from fileStructure
externUsages.o: 	setting.o vector.o codeSeg.o linkedList.o fileStructures.o
fileStructures.o: 	symbolTable.o dataSeg.o codeSeg.o entryLines.o errors.o
entryLines.o: 		setting.o text.o vector.o linkedList.o symbolTable.o
codeSeg.o: 			setting.o text.o vector.o machineWord.o
dataSeg.o: 			setting.o vector.o machineWord.o
symbolTable.o: 		setting.o linkedList.o

# dependencies of files from utils
errors.o: text.o
text.o: setting.o
machineWord.o: setting.o

# setting.o, linkedList.o and vector.o are not depend on anything

############################ Clean rules
clean:
	rm -f *.o assembler

# Clean assembler output
cleanOutput:
	rm -f *.am *.ob *ext *ent