/**
 * @file symbolTable.h
 * @brief Defines structures and functions related to symbol tables
 * used in the assembly language processing.
 *
 * It provides functionalities for creating, adding, finding, printing, and
 * freeing symbols in the symbol table. <br>
 * Additionally, it includes a function to update symbol addresses after the
 * first pass and the instruction counter calculation.
 */

#ifndef _MY_SYMBOL_TABLE_H_
#define _MY_SYMBOL_TABLE_H_

#include "../setting.h"
#include "../utils/linkedList.h"

/** Enumeration representing the type of a symbol. */
typedef enum SymbolType
{
    DATA, CODE, DEFINE, EXTERN,
    UNRESOLVED_USAGE, UNRESOLVED_ENTRY, UNRESOLVED_ENTRY_USAGE
} SymbolType;

/** Enumeration representing the flag for an entry. */
typedef enum EntryFlag
{
    NOT_ENTRY, IS_Entry
} EntryFlag;

/**
 * @struct Struct representing data associated with a symbol.
 */
typedef struct Symbol_Data
{
    int val;               /**< Address of the symbol or value if it's define */
    SymbolType type;       /**< Type of the symbol. */
    EntryFlag isEntry;     /**< Flag indicating whether the symbol is an entry. */
} Symbol_Data;

typedef Node Symbol;

typedef struct Symbol_Table
{
    LinkedList *database;         /**< Linked list storing symbol data. */
    int extern_count;             /**< Count of external symbols. */
    int unresolved_entry_count;   /**< Count of unresolved entry symbols. */
    int unresolved_usage_count;   /**< Count of unresolved usage symbols. */
} Symbol_Table;

/**
 * @brief Creates a new symbol table.
 *
 * @return Pointer to the newly created symbol table,
 *          or NULL if memory allocation fails.
 */
Symbol_Table *new_symbol_table (void);

/**
 * @brief Adds a symbol to the symbol table.
 *
 * @param table   Pointer to the symbol table.
 * @param label   The label of the symbol.
 * @param type    Type of the symbol.
 * @param val     Value or address of the symbol.
 * @param isEntry Flag indicating whether the symbol is an entry.
 * @return        Pointer to the newly added symbol,
 *                or NULL if memory allocation fails.
 */
Symbol *add_symbol (Symbol_Table *table, const char *label,
                    SymbolType type, int val, EntryFlag isEntry);

/**
 * @brief Finds a symbol in the symbol table.
 *
 * @param table Pointer to the symbol table.
 * @param name  The name of the symbol to find.
 * @return      Pointer to the found symbol,
 *              or NULL if the symbol is not found.
 */
Symbol *find_symbol (Symbol_Table *table, const char *name);

/**
 * @brief Display the entire symbol table.
 *
 * @param table  Pointer to the symbol table to print.
 * @param stream File stream to print to.
 */
void display_symbol_table (Symbol_Table *table, FILE *stream);

/**
 * Prints the entire symbol table.
 *
 * @param table  Pointer to the symbol table to print.
 * @param stream File stream to print to.
 */
void free_symbol_table (Symbol_Table *table);

/**
 * @brief Updates symbol addresses after the first pass ended.
 *        add the start point of the ic for all the symbols,
 *        and the ic as calculated to data symbols.
 *
 * @param symbol_table Pointer to the symbol table.
 * @param IC           Instruction Counter that will be added to data symbols
 */
void update_data_symbol_addresses (Symbol_Table *symbol_table, int IC);

#endif /* _MY_SYMBOL_TABLE_H_ */
