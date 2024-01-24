/*
 Created by OMER on 1/23/2024.
*/


#ifndef _SETTING_H_
#define _SETTING_H_

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define NUM_OF_OP 16
#define NUM_OF_ADDRESSING_MODE 4
#define MAX_CMD_NAME_LEN 3

/*todo change */
typedef enum Bool
{
    FALSE = 0, TRUE = 1
} Bool;

typedef enum exit_code
{
    SUCCESS = 0, ERROR = 1, FAILURE = 2
}exit_code;

typedef enum Opcode
{
    /*first group - 2 operators*/
    MOV = 0,
    CMP = 1,
    ADD = 2,
    SUB = 3,
    LEA = 6,

    /*second group - 1 operators*/
    NOT = 4,
    CLR = 5,
    INC = 7,
    DEC = 8,
    JMP = 9,
    BNE = 10,
    RED = 11,
    PRN = 12,
    JSR = 13,

    /* third group - no operators */
    RTS = 14,
    HLT = 15
} Opcode;

typedef enum Operand_Add_Mode
{
    NONE_MODE = -1,
    IMMEDIATE_MODE = 0,
    DIRECT_MODE = 1,
    INDEX_MODE = 2,
    REGISTER_MODE = 3
} Operand_Add_Mode;

typedef struct Op_Info
{
    Opcode opcode;
    char name[MAX_CMD_NAME_LEN];
    Bool src_modes[NUM_OF_ADDRESSING_MODE];
    Bool target_modes[NUM_OF_ADDRESSING_MODE];
} Op_Info;

typedef enum Register
{
    R0 = 0, R1, R2, R3, R4, R5, R6, R7, NO_REG=-1
} Register;

void init_assembler_setting ();

extern Op_Info op_info[NUM_OF_OP];

#endif /* _SETTING_H_ */
