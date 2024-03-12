/*
 Created by OMER on 1/23/2024.
*/


#ifndef _SETTING_H_
#define _SETTING_H_

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "utils/vector.h"
#include "utils/linkedList.h"

#define MAX_LINE_SIZE (80+2)
#define MAX_TOKEN_SIZE (80+2)
#define NUM_OF_OP 16
#define NUM_OF_ADDRESSING_MODE 4
#define MAX_CMD_NAME_LEN 3

#define MACHINE_WORD_SIZE 14
#define INIT_IC 100

/*todo change */
typedef enum Bool
{
    FALSE = 0, TRUE = 1
} Bool;

typedef enum exit_code
{
    MEMORY_ERROR = -1, ERROR = 0, SUCCESS = 1
}exit_code;

typedef enum Opcode
{
    NO_OPCODE = -1,

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

typedef enum Addressing_Mode
{
    NONE_ADD = -1,
    IMM_ADD = 0,
    DIRECT_ADD = 1,
    INDEX_ADD = 2,
    REG_ADD = 3
} Addressing_Mode;

typedef struct Op_Propriety
{
    Opcode opcode;
    char name[MAX_CMD_NAME_LEN];
    Bool src_modes[NUM_OF_ADDRESSING_MODE];
    Bool target_modes[NUM_OF_ADDRESSING_MODE];
} Op_Propriety;

typedef enum Register
{
    R0 = 0, R1, R2, R3, R4, R5, R6, R7, INVALID_REG=-1
} Register;

typedef struct file_analyze{
    char file_name[1000]; /* todo change size*/
    LinkedList *macro_list; //delete?
    LinkedList *symbol_table; //
    vector *data_segment;
    vector *code_segment;
    vector *op_list;
    vector *entry_table;
    vector *extern_table;
    size_t IC;
    size_t DC;

    exit_code error;
}file_analyze;

void free_all(int num, ...);

void init_assembler_setting ();

extern Op_Propriety op_propriety[NUM_OF_OP];
extern char *SavedWord[];

#endif /* _SETTING_H_ */
