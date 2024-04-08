/**
 * @file codeSeg.h
 * @brief Defines structures and functions for code segment analysis during assembly.
 *
 * During the first pass of assembly, each instruction line in the code is
 * stored in Op_Line, which includes analysis (Op_Analyze) and the original
 * line (LineParts). <br>
 * These are added to Op_List, enabling further examination in the second
 * pass, such as resolving unresolved symbols.  <br>
 *
 * This file contains the necessary structures and functions for construction,
 * updating, and managing Op_Line, Op_Analyze, and Op_List structures. <br>
 * Additionally, contain function for printing the code segment from the
 * Op_List.
 */

#ifndef _CODE_SEG_H_
#define _CODE_SEG_H_

#include "../setting.h"
#include "../utils/text.h"
#include "../utils/vector.h"

/* ====================================================================
 *                              op analyze
 * ==================================================================== */

/**
 * @typedef Operand_Type
 * @brief Type definition for operand types.
 */
typedef enum Operand_Type
{
    SRC, TARGET
} Operand_Type;

/**
 * @struct Operand
 * @brief representing an operand and its various modes.
 */
typedef struct Operand
{
    Operand_Type type;        /** < Type of the operand */
    unsigned param_types: 4;  /** < All the valid addressing mode for the operand */
    Addressing_Mode add_mode; /** < Addressing mode */
    union                     /** < Union of all the possible modes */
    {
        int reg_num;    /* < for register mode */
        int imm;        /* < for immediate mode */
        struct          /* < for symbol or index mode */
        {
            void *symbol; /* pointer to the symbol */
            Bool found;   /* Indicates if the symbol is found */
            int offset;   /* Offset (0 in direct mode) */
        } symInx;
    } info;
} Operand;

/**
 * @struct Op_Analyze
 * @brief Structure representing an analyzed operation.
 */
typedef struct Op_Analyze
{
    Opcode opcode;    /**<  Opcode of the operation */
    int address;      /**<  Memory address of the operation */
    Operand src;      /**<  Source operand (might be null) */
    Operand target;   /**<  Target operand (might be null) */
} Op_Analyze;

/**
 * @brief Initializes an operation analysis structure.
 *
 * @param op                Pointer to the operation analysis structure to initialize.
 * @param opcode            Opcode of the operation.
 * @param src_sym_buffer    buffer to hold name of the symbol use in src
 *                          operand in case it unresolved
 * @param target_sym_buffer buffer to hold name of the symbol use
 *                          in target operand in case it unresolved
 *
 * @note both buffer will be used only if the address mode is direct/index,
 *      and only if the symbol is unresolved
 */
void init_op_analyze (Op_Analyze *op, Opcode opcode, char *src_sym_buffer,
                      char *target_sym_buffer);

/**
 * @brief Calculates the size of the operation in the memory image
 *
 * @param op Pointer to the operation analysis structure.
 * @return The size of the operation.
 */
int calc_op_size (Op_Analyze *op);

/* ====================================================================
 *                              op line
 * ==================================================================== */
/**
 * @struct Op_Line
 * @brief representing an operation line. Hold it analyze and its original
 * line in the code.
 */
typedef struct Op_Line
{
    Op_Analyze *analyze; /**< Pointer to the analyzed operation */
    LineParts *parts;    /** < Pointer to the line parts */
} Op_Line;

typedef vector Op_List;

/**
 * @brief Creates a new operation list.
 *
 * @return Pointer to the newly created operation list, or null if memory
 * allocation fails.
 */
Op_List *new_op_list (void);

/**
 * @brief Adds an operation line to the operation list.
 *
 * @param op_list       Pointer to the operation list.
 * @param op_analyze    Pointer to the analyzed operation.
 * @param line_part     Pointer to the line parts.
 * @return              Pointer to the added operation line,
 *                      or null if memory allocation fails.
 */
Op_Line *add_to_op_list (Op_List *op_list, Op_Analyze *op_analyze,
                         LineParts *line_part);

/**
 * @brief Displays the information of an operation line.
 *
 * @param op_list   Pointer to the operation list.
 * @param stream    File stream to print to.
 */
void display_op_list (Op_List *op_list, FILE *stream);


/* ====================================================================
 *                              code segment
 * ==================================================================== */
/**
 * @brief Prints the code segment in the machine language
 *
 * @param op_list Pointer to the operation list.
 * @param memInx Memory index.
 * @param len Length of the code segment.
 * @param stream File stream to print to.
 */
void print_code_segment (Op_List *op_list, int memInx, int len, FILE *stream);

/**
 * @brief Frees the memory allocated for the operation list.
 *
 * @param op_list Pointer to the operation list.
 */
void free_op_list (Op_List *op_list);

#endif //_CODE_SEG_H_
