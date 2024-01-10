cmake_minimum_required(VERSION 3.22)
project(project___mama14 C)

set(CMAKE_C_STANDARD 90)

#add_executable(project___mama14
#        test.c
#        linkedList.c linkedList.h
#        input.c input.h
#        preAssembler.c preAssembler.h)


add_executable(project___mama14
        linkedList.c linkedList.h
        input.c input.h
        preAssembler.c preAssembler.h
        passOne.c passOne.h
        main.c)


