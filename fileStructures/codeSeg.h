/**
 * @file codeSeg.h
 * @brief Defines structures and functions for code segment analysis during assembly.
 *
 * During the first pass of assembly, each instruction line in the code is
 * stored in OpLine, which includes analysis (OpAnalyze) and the original
 * line (LineParts). <br>
 * These are added to OpLinesList, enabling further examination in the second
 * pass, such as resolving unresolved symbols.  <br>
 *
 * This file contains the necessary structures and functions for construction,
 * updating, and managing OpLine, OpAnalyze, and OpLinesList structures. <br>
 * Additionally, contain function for printing the code segment from the
 * OpLinesList.
 */

#ifndef _CODE_SEG_H_
#define _CODE_SEG_H_

#include "../setting.h"
#include "../utils/text.h"


/* ====================================================================
 *                              op analyze
 * ==================================================================== */

/**
 * @typedef Operand_Type
 * @brief Type definition for operand types.
 */
typedef enum OperandType
{
    SRC = 1, TARGET = 2
    /* it 1,2 because it's the also colum the index in the param type table */
} OperandType;

/**
 * @struct Operand
 * @brief representing an operand and its various modes.
 */
typedef struct Operand
{
    OperandType type;        /** < Type of the operand */
    unsigned param_types: 4;  /** < All the valid addressing mode for the operand */
    AddressingMode add_mode; /** < Addressing mode */
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
typedef struct OpAnalyze
{
    Opcode opcode;    /**<  Opcode of the operation */
    int address;      /**<  Memory address of the operation */
    Operand src;      /**<  Source operand (might be null) */
    Operand target;   /**<  Target operand (might be null) */
} OpAnalyze;

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
void init_op_analyze (OpAnalyze *op, Opcode opcode, char *src_sym_buffer,
                      char *target_sym_buffer);

/**
 * @brief Calculates the size of the operation in terms of the number of lines
 * it occupies in the memory image when written as machine code.
 *
 * @param op Pointer to the operation analysis structure.
 * @return The size of the operation.
 */
int calc_op_size (OpAnalyze *op);

/**
 * @brief Displays the information of an operation line.
 *
 * @param op_line   Pointer to the operation line structure to display.
 * @param stream    File stream to which the information will be written.
 */
void display_op_line (const void *op_line, FILE *stream);

/* ====================================================================
 *                              op line
 * ==================================================================== */
/**
 * @struct OpLine
 * @brief representing an operation line. Hold its analysis, and its original
 * line in the code.
 */
typedef struct OpLine
{
    OpAnalyze *analyze; /**< Pointer to the analyzed operation */
    LineParts *parts;    /** < Pointer to the line parts */
} OpLine;


/**
* @typedef OpLinesList
* @brief A typedef representing a list of operation lines.
*
* The structure holds all the operation lines that did not encounter an error
 * on the first pass.
* Its purpose is to save these lines to perform additional error analysis,
 * of forward declaration of labels in the second pass.
 * If no additional errors are detected, the code segment will be generated
 * from it.
*/
typedef struct OpLinesList OpLinesList;

/**
 * @brief Creates a new operation lines list.
 *
 * @return Pointer to the newly created operation list, or null if memory
 * allocation fails.
 */
OpLinesList *new_op_lines_list (void);

/**
 * @brief Adds an operation line to the operation lines list.
 *
 * @param op_list       Pointer to the operation list.
 * @param op_analyze    Pointer to the analyzed operation.
 * @param line_part     Pointer to the line parts.
 * @return              Pointer to the added operation line,
 *                      or null if memory allocation fails.
 */
OpLine *add_to_op_lines_list (OpLinesList *op_list, OpAnalyze *op_analyze,
                              LineParts *line_part);

/**
 * @brief Retrieves a pointer to the line at the specified index in the
 *        operation lines list.
 *
 * @param op_list   Pointer to the operation lines list.
 * @param i         Index of the line to retrieve.
 * @return          A pointer to the line at the specified index, or NULL if
 *                  the index is out of range.
 */
OpLine* get_op_line(OpLinesList *op_list, int i);

/**
 * @brief Displays the information of the operation lines list.
 *
 * @param op_list   Pointer to the operation lines list.
 * @param stream    File stream to print to.
 */
void display_op_lines_list (OpLinesList *op_list, FILE *stream);


/* ====================================================================
 *                              code segment
 * ==================================================================== */
/**
 * @brief Prints the code segment in the machine language
 *
 * @param op_list Pointer to the operation lines list.
 * @param memInx Memory index.
 * @param len Length of the code segment.
 * @param stream File stream to print to.
 */
void print_code_segment (OpLinesList *op_list, int memInx, int len, FILE *stream);

/**
 * @brief Frees the memory allocated for the operation lines list.
 *
 * @param op_list Pointer to the operation list.
 */
void free_op_lines_list (OpLinesList *op_list);

#endif /* _CODE_SEG_H_ */
