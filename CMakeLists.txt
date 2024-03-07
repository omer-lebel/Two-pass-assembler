cmake_minimum_required(VERSION 3.22)
project(project___mama14 C)

set(CMAKE_C_STANDARD 90)

#add_executable(project___mama14
#        test.c
#        linkedList.c linkedList.h
#        input.c input.h
#        preAssembler.c preAssembler.h)


add_executable(project___mama14
        setting.c setting.h

        utils/linkedList.c utils/linkedList.h
        utils/text.c utils/text.h

        fileStructures/symbolTable.c fileStructures/symbolTable.h
        fileStructures/memoryImg.c fileStructures/memoryImg.h
        fsm.c fsm.h

        preAssembler.c preAssembler.h
        firstPass.c firstPass.h

        main.c utils/vector.c utils/vector.h secondPass.c secondPass.h utils/machineWord.c utils/machineWord.h fileStructures/entryTable.c fileStructures/entryTable.h fileStructures/ast.c fileStructures/ast.h fileStructures/externTable.c fileStructures/externTable.h)


