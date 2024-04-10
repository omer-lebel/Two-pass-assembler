/**
 * @file entryLines.h
 * @brief Manages the list of entry lines containing symbols declared as entry
 */

#ifndef _ENTRY_LINES_H_
#define _ENTRY_LINES_H_

#include "symbolTable.h"
#include "../utils/text.h"
#include "../utils/vector.h"

/**
 * @struct Entry_line
 * @brief Represents a line containing a symbol declared as an entry point.
 * for exp: .entry LABEL
 */
typedef struct Entry_line{
    Symbol *symbol;     /**< Pointer to the symbol declared as an entry point. */
    LineParts *parts;   /**< Pointer to the line parts, representing the entry declaration. */
}Entry_line;
/*
 * If the symbol is unresolved when added, the 'LineParts' field will hold the
 * LineParts in order to be able to print an error.
 * Otherwise the symbol is resolved, so the 'LineParts' field will be null.
 */

typedef vector Entry_List;

/**
 * @brief Creates a new entry list.
 *
 * @return Pointer to the newly created entry list, or null if memory allocation fails.
 */
Entry_List *new_entry_list (void);

/**
 * @brief Adds a line containing a symbol declared as an entry to the entry list.
 *
 * @param entry_list Pointer to the entry list.
 * @param symbol Pointer to the symbol declared as an entry point.
 * @param line_part Pointer to the line parts representing the entry declaration.
 * @return Pointer to the added entry line if successful, NULL otherwise.
 */
Entry_line *add_to_entry_list (Entry_List *entry_list, Symbol *symbol,
                               LineParts *line_part);

/**
 * @brief Prints the entry list to a file stream (typically to .ent file)
 * For each entry symbol print its name and its line in the memory image.
 *
 * @param entry_list Pointer to the entry list.
 * @param stream File stream to print to.
 */
void print_entry_list (Entry_List *entry_list, FILE *stream);

/**
 * @brief Frees the memory allocated for the entry list.
 *
 * @param entry_list Pointer to the entry list to free.
 */
void free_entry_list (vector *entry_list);

#endif /* _ENTRY_LINES_H_ */
