/**
 * @file analysis.h
 * @brief Declarations for syntax and semantic analysis functions,
 *        along with structures and helpers for handling line information.
 *
 * This header file contains declarations for functions responsible for
 * syntax and semantic analysis. It also defines structures and helper
 * functions to manage line information during the analysis process.
 *
 * @Note: Some analysis functions in this module handle error reporting
 *       as part of their operation.
 */

#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_

/* ------------------------------- includes ------------------------------- */
#include "setting.h"
#include "utils/text.h"
#include "utils/errors.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/codeSeg.h"
/* ------------------------------- defines -------------------------------- */
#define IS_CLOSE_INDEX_SIGN(s) (strcmp (s, "]") == 0)
#define CHAR_TO_INT(c) ((c) - '0')
#define IN_REG_BOUND(s) ((s) >= '0' && (s) <= '7')
#define IS_COMMA(s) ((s) == ',')
#define IS_IMM_SIGN(s) ((s) == '#')
#define IS_OPEN_INDEX_SIGN(s) (s == '[')
/* ------------------------------------------------------------------------- */

/* =======================================================================
 *                             line info
 * ======================================================================= */

/**
 * @brief Enumeration representing the type of a line.
 */
typedef enum lineType
{
    str_line,         /**< .string */
    data_line,        /**< .data */
    ext_line,         /**< .extern */
    ent_line,         /**< .entry */
    op_line,          /**< operator */
    def_line          /**< .define */
} lineType;


/**
 * @brief Union representing information about a line according to its type.
 */
typedef union Info
{
    OpAnalyze *op;    /**< operator info */

    struct            /**< data info */
    {
        int *arr;
        int len;
    } data;

    struct            /**< string info */
    {
        char *content;
        int len;
    } str;

    struct          /**< define info */
    {
        void *name;
        Bool found;
        int val;
    } define;

    struct        /** entry or extern */
    {
        void *name;
        Bool found;
    } ext_ent;
} Info;

/**
 * @typedef LineInfo
 * @brief Structure representing information about a line.
 */
typedef struct LineInfo
{
    lineType type;     /** Type of the line: str / data / ext / ent / def /op */
    LineParts *parts;  /** Parts of the line: postfix, token, postfix, number */
    char *label;       /** Label of the line, if it exists */
    Info info;         /** Information about the line */

} LineInfo;

/**
 * @brief display LineInfo structure.
 * @param line Pointer to the LineInfo structure containing information about the line.
 */
void display_line_info (LineInfo *line);

/**
 * @brief Restart the LineInfo structure, clearing its contents.
 * @param line Pointer to the LineInfo structure to be restarted.
 */
void restart_line_info (LineInfo *line);

/* =======================================================================
 *                        Syntax analysis helpers
 * ======================================================================= */

/**
 * @brief Check if a string represents a register (r0 - r7)
 * @param str       Pointer to the string to be checked.
 * @param reg_num   Pointer to an integer to store the register number if the string represents a register.
 * @return TRUE if the string represents a register, otherwise FALSE.
 */
Bool is_reg (const char *token, int *reg_num);

/**
 * @brief Check if a token represents a valid string declaration - starts
 * and ends with double quotes.
 *
 * @param line  The LineParts structure containing the line token to be checked.
 * @return Returns TRUE if the line represents a valid string declaration, otherwise FALSE.
 */
Bool is_str (LineParts *line);

/**
 * @brief Checks if a token represents a valid integer.
 *
 * @param token The token to be checked.
 * @param imm Pointer to store the converted integer value.
 * @return Returns TRUE if the token represents a valid integer, otherwise FALSE.
 */
Bool is_int (char *token, int *imm);

/**
 * @brief Check if a string represents a defined constant.
 *
 * @param line          Pointer to the LineInfo structure containing information about the line.
 * @param token         Pointer to the token to be checked.
 * @param symbol_table  Pointer to the symbol table.
 * @param imm           Pointer to an integer to store the value of the defined constant if the token represents a defined constant.
 * @return TRUE if the token represents a defined constant, otherwise FALSE.
 */
Bool is_define_constant (LineInfo *line, char *token, Symbol_Table *symbol_table,
                         int *imm);

/**
 * @brief Check if a string represents an immediate value.
 *
 * @param line      Pointer to the LineInfo structure containing information about the line.
 * @param token     Pointer to the token to be checked.
 * @param table     Pointer to the symbol table.
 * @param imm       Pointer to an integer to store the immediate value if the token represents an immediate value.
 * @return TRUE if the token represents an immediate value, otherwise FALSE.
 */
Bool is_imm (LineInfo *line, char *token, Symbol_Table *table, int *imm);

/**
 * @brief Check if a string is a valid identifier.
 *
 * 1) Starts with alphabetic char <br>
 * 2) Contain only alphanumeric characters <br>
 * 3) Is not a reserve word of the language
 * @param line      Pointer to the LineParts structure containing information about the line.
 * @param name      Pointer to the name of the identifier to be checked.
 * @param print_err Flag indicating whether to print errors.
 * @return TRUE if the string is a valid identifier, otherwise FALSE.
 */
Bool is_valid_identifier (LineParts *line, char *name, Bool print_err);

/**
 * @brief Checks if a token fits direct address mode.
 *
 * Determines if the token represents an existing symbol or an unresolved one. <br>
 * If an existing symbol is found and is not defined, it updates the operand
 * with symbol information, sets 'found' to TRUE, and update unresolved entry
 * count if needed. <br>
 * Otherwise, if the token has the correct syntax for a symbol, it's assumed
 * to be unresolved and copied as the symbol name into the operand, setting
 * 'found' to FALSE.
 *
 * @param line          Pointer to the LineInfo structure containing information about the line.
 * @param name          Pointer to the name of the symbol to be checked.
 * @param symbol_table  Pointer to the symbol table.
 * @param operand       Pointer to the Operand structure to store information about the symbol.
 * @return TRUE if the string represents a symbol, otherwise FALSE.

 */
Bool is_symbol (LineInfo *line, char *name, Symbol_Table *symbol_table,
                Operand *operand);

/**
 * @brief Checks if a token fits index address mode: symbol[imm]
 *
 * @param line      Pointer to the LineInfo structure containing information about the line.
 * @param name      Pointer to the name of the symbol to be checked.
 * @param table     Pointer to the symbol table.
 * @param operand   Pointer to the Operand structure to store information about the index.
 * @return TRUE if the string represents an index, otherwise FALSE.

 */
Bool is_index (LineInfo *line, char *name, Symbol_Table *table, Operand *operand);

/**
 * @brief Check if a string represents a closing index sign ("]").
 * @param line  Pointer to the LineInfo structure containing information about the line.
 * @return TRUE if the string represents a closing index sign, otherwise FALSE.
 */
Bool is_closing_index_sign (LineInfo *line);

/* =======================================================================
 *                      Semantic analysis helpers
 * ======================================================================= */
/**
 * @brief Check if an addressing mode is valid for an operand.
 *
 * @param line      Pointer to the LineInfo structure containing information about the line.
 * @param operand   Pointer to the Operand structure to be validated.
 * @param add_mode  Addressing mode flag to be validated.
 * @return          TRUE if the addressing mode is valid, otherwise FALSE.
 */
Bool valid_add_mode (LineInfo *line, Operand *operand, addr_mode_flag add_mode);

/**
 * @brief Validates an existing entry label declaration.
 *
 * Checks if the declaration of an existing entry label is valid. <br>
 * - If the original label is code data declaring - it's valid. <br>
 * - If it's redeclaration of entry label - a warning is issued.
 * - Otherwise (if the original label is external or defined), it's error
 *
 * @param line      Pointer to the LineInfo structure containing information about the line.
 * @param symbol    Pointer to the Symbol structure representing the entry label.
 * @return TRUE if the entry label is valid, otherwise FALSE.
 *
 */
Bool valid_entry_label (LineInfo *line, Symbol *symbol);

/**
 * @brief Validates an existing extern label declaration.
 *
 * Checks if the declaration of an existing extern label is valid. <br>
 * - If it's redeclaration of entry label - valid but a warning is issued. <br>
 * - otherwise - it's an error
 *
 * @param line      Pointer to the LineInfo structure containing information about the line.
 * @param symbol    Pointer to the Symbol structure representing the extern label.
 * @return TRUE if the extern label is valid, otherwise FALSE.
 *
 * @errors - redeclaration_err
 *         - duplicate_declaration_warning
 */
Bool valid_extern_label (LineInfo *line, Symbol *symbol);

/**
 * @brief Validates an existing define constant declaration.
 *
 * Checks if the declaration of an existing define is valid. <br>
 * - If it's redeclaration of define label, it assumes it's going to have
 * the same value and considers it valid (for now).
 * - otherwise - it's considered an error.
 * @param line      Pointer to the LineInfo structure containing information about the line.
 * @param symbol    Pointer to the Symbol structure representing the define label.
 * @return TRUE if the define label is valid, otherwise FALSE.
 */
Bool valid_define_symbol (LineInfo *line, Symbol *symbol);



#endif /* _ANALYSIS_H_ */
