/*
 Created by OMER on 1/23/2024.
*/
#define DEBUG



#ifndef _SETTING_H_
#define _SETTING_H_

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "utils/vector.h"
#include "fileStructures/symbolTable.h"

#define MAX_LINE_LEN (80+2)
#define MAX_DATA_ARR_LENGTH (37)
#define MAX_STR_LENGTH (72)
#define NUM_OF_OP 16
#define MAX_CMD_NAME_LEN 3

#define MACHINE_WORD_SIZE 14
#define IC_START 100

/*todo change */
typedef enum Bool
{
    FALSE = 0, TRUE = 1
} Bool;

typedef enum exit_code
{
    SUCCESS = 0, ERROR = 1, MEMORY_ERROR = 2
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

typedef enum ARE
{
    external_b = 1, relocatable_b = 2
} ARE;

typedef enum Addressing_Mode
{
    NONE_ADD = -1,
    IMM_ADD = 0,
    DIRECT_ADD = 1,
    INDEX_ADD = 2,
    REG_ADD = 3
} Addressing_Mode;

typedef enum addr_mode_flag
{
    b_imm = 8, /* 1000 */
    b_symbol = 4, /* 0100 */
    b_index = 2, /* 0010 */
    b_reg = 1 /* 0001 */
} addr_mode_flag;

#define no_operand 0
#define all_mode ((unsigned) b_imm | b_symbol | b_index | b_reg)
#define symbol_n_reg ((unsigned) b_symbol | b_reg)
#define symbol_n_index ((unsigned) b_symbol | b_index )
#define symbol_n_index_n_reg ((unsigned) b_symbol | b_index | b_reg)

typedef struct file_analyze{
    char file_name[1000]; /* todo change size*/
    Symbol_Table *symbol_table;
    vector *data_segment;
    vector *code_segment;
    vector *op_list;
    vector *entry_table;
    vector *extern_table;
    int IC; /* todo change to unsigned? */
    int DC;

    unsigned error;
}file_analyze;

FILE* open_file(char* file_name, const char *extension, const char *mode);
void remove_file(char* file_name, const char *extension);
/* todo change place */

extern char *SavedWord[];
extern char *op_names[NUM_OF_OP];
extern unsigned int param_types[NUM_OF_OP][MAX_CMD_NAME_LEN];

#endif /* _SETTING_H_ */
