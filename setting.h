/**
 * @file setting.h
 * Global settings and definitions for the assembler program.
 */
#ifndef _SETTING_H_
#define _SETTING_H_

/* ------------------------------- includes ------------------------------- */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
/* -------------------------------- defines -------------------------------- */

#define IC_START 100
#define MACHINE_WORD_SIZE 14
#define MEMORY_IMG_SIZE (4096 - 100 - 1)
#define MAX_LINE_LEN (80+2)
#define MAX_LABEL_LEN (31)
#define MAX_DATA_ARR_LENGTH (37)
#define MAX_STR_LENGTH (72)
#define NUM_OF_OP 16
#define MAX_CMD_NAME_LEN 3

/**
 * @note The maximum and minimum numbers were determined based on the use of
 * 12 bits with sign, which is the possible limit that can be expressed in a
 * number as an operand.
 */
#define MAX_INT_MACHINE ((1 << 11) - 1)
#define MIN_INT_MACHINE (-(1 << 11))

/** Uncomment to see more output from the assembler's data structures.
 * (first and second pass output are the same expect the symbol table) */
/*#define DEBUG_PRE*/
/*#define DEBUG_FIRST_PASS*/
/*#define DEBUG_SECOND_PASS*/

/* ------------------------------ global vars ------------------------------ */
extern char *reserved_words[];
extern char *operation_names[NUM_OF_OP];
extern unsigned int addr_mode_table[NUM_OF_OP][MAX_CMD_NAME_LEN];

/* -------------------------------- typedef -------------------------------- */

typedef enum Bool
{
    FALSE = 0, TRUE = 1
} Bool;

/**
 * @brief Exit codes for function in the assembler.
 */
typedef enum exit_code
{
    SUCCESS = 0,      /**< Successful execution, no errors are found. */
    ERROR = 1,        /**< Found error during process. */
    MEMORY_ERROR = 2  /**< Memory allocation error. */
}exit_code;

/**
 * @brief Operation codes (opcodes) enumeration.
 */
typedef enum Opcode
{
    NO_OPCODE = -1,

    /*first group - 2 operators*/
    MOV = 0, CMP = 1, ADD = 2, SUB = 3, LEA = 6,

    /*second group - 1 operators*/
    NOT = 4, CLR = 5, INC = 7, DEC = 8, JMP = 9,
    BNE = 10, RED = 11, PRN = 12, JSR = 13,

    /* third group - no operators */
    RTS = 14, HLT = 15
} Opcode;

typedef enum ARE
{
    external_b = 1, relocatable_b = 2
} ARE;

/**
 * @brief Addressing modes enumeration.
 */
typedef enum Addressing_Mode
{
    NONE_ADD = -1,
    IMM_ADD = 0,
    DIRECT_ADD = 1,
    INDEX_ADD = 2,
    REG_ADD = 3
} AddressingMode;

/**
 * @brief Addressing mode bit flags for operands.
 */
typedef enum addr_mode_flag
{
    b_imm = 8,      /* 1000 */
    b_symbol = 4,   /* 0100 */
    b_index = 2,    /* 0010 */
    b_reg = 1       /* 0001 */
} addr_mode_flag;

/* ---------------------- defines of addressing modes ---------------------- */
/**
 * @brief Defines related to addressing modes and operands.
 * Used to utilized in the addressing mode table and operand configurations
 */
#define no_operand 0
#define all_mode ((unsigned) b_imm | b_symbol | b_index | b_reg)
#define symbol_n_reg ((unsigned) b_symbol | b_reg)
#define symbol_n_index ((unsigned) b_symbol | b_index )
#define symbol_n_index_n_reg ((unsigned) b_symbol | b_index | b_reg)

/* ------------------------------------------------------------------------- */




#endif /* _SETTING_H_ */
