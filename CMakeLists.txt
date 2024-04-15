cmake_minimum_required(VERSION 3.22)
project(project___mama14 C)

set(CMAKE_C_STANDARD 90)


add_executable(project___mama14
        setting.c setting.h

        utils/errors.c utils/errors.h
        utils/linkedList.c utils/linkedList.h
        utils/machineWord.c utils/machineWord.h
        utils/text.c utils/text.h
        utils/vector.c utils/vector.h

        fileStructures/fileStructures.h fileStructures/fileStructures.c
        fileStructures/symbolTable.c fileStructures/symbolTable.h
        fileStructures/codeSeg.c fileStructures/codeSeg.h
        fileStructures/dataSeg.c fileStructures/dataSeg.h
        fileStructures/entryLines.h fileStructures/entryLines.c
        fileStructures/externUsages.c fileStructures/externUsages.h

        preAssembler.c preAssembler.h
        analysis.c analysis.h
        fsm.c fsm.h
        firstAnalysis.c firstAnalysis.h
        firstPass.c firstPass.h
        secondPass.c secondPass.h

        assembler.c)


