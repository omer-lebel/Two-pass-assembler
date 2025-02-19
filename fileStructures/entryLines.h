/**
 * @file entryLines.h
 * @brief Manages the list of entry lines containing symbols declared as entry
 */

#ifndef _ENTRY_LINES_H_
#define _ENTRY_LINES_H_

#include "symbolTable.h"
#include "../utils/text.h"

/**
 * @struct EntryLine
 * @brief Represents a line containing a symbol declared as an entry point.
 * for exp: .entry LABEL
 */
typedef struct EntryLine{
    Symbol *symbol;     /**< Pointer to the symbol declared as an entry point. */
    LineParts *parts;   /**< Pointer to the line parts, representing the entry declaration. */
}EntryLine;
/*
 * If the symbol is unresolved when added, the 'LineParts' field will hold the
 * LineParts in order to be able to print an error.
 * Otherwise the symbol is resolved, so the 'LineParts' field will be null.
 */


/**
 * @typedef EntryLinesList
 * @brief
 *
 */
typedef struct EntryLinesList EntryLinesList;

/**
 * @brief Creates a new entry list.
 *
 * @return Pointer to the newly created entry list, or null if memory allocation fails.
 */
EntryLinesList *new_entry_lines_list (void);


/**
 * @brief Checks if the entry lines list is empty.
 *
 * @param entry_list Pointer to the entry lines list to check.
 * @return           TRUE if the entry lines list is empty, FALSE otherwise.
 */
Bool is_empty(EntryLinesList* entry_list);

/**
 * @brief Adds a line containing a symbol declared as an entry to the entry list.
 *
 * @param entry_list Pointer to the entry list.
 * @param symbol Pointer to the symbol declared as an entry point.
 * @param line_part Pointer to the line parts representing the entry declaration.
 * @return Pointer to the added entry line if successful, NULL otherwise.
 */
EntryLine *add_to_entry_lines_list (EntryLinesList *entry_list, Symbol *symbol,
                                    LineParts *line_part);

/**
 * @brief Retrieves a pointer to the line at the specified index in the
 *        entry lines list.
 *
 * @param op_list   Pointer to the entry lines list.
 * @param i         Index of the line to retrieve.
 * @return          A pointer to the line at the specified index, or NULL if
 *                  the index is out of range.
 */
EntryLine * get_entry_line(EntryLinesList *entry_list, int i);

/**
 * @brief Prints the entry list to a file stream (typically to .ent file)
 * For each entry symbol print its name and its line in the memory image.
 *
 * @param entry_list Pointer to the entry list.
 * @param stream File stream to print to.
 */
void print_entry_list (EntryLinesList *entry_list, FILE *stream);

/**
 * @brief Frees the memory allocated for the entry list.
 *
 * @param entry_list Pointer to the entry list to free.
 */
void free_entry_lines_list (EntryLinesList *entry_list);

#endif /* _ENTRY_LINES_H_ */
