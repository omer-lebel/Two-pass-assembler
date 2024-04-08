/**
 * @file fileStructures.h
 * @brief Header file containing structures and functions for file analysis
 * in the assembler.
 */

#ifndef _FILE_STRUCTURES_H_
#define _FILE_STRUCTURES_H_


#include "../utils/errors.h"
#include "symbolTable.h"
#include "dataSeg.h"
#include "codeSeg.h"
#include "entryLines.h"

#define MAX_LEN_FILE_NAME 256 /* The maximum length of a pathname in most os */

/**
 * @struct file_analyze
 * @brief Structure containing information about a file being analyzed by
 * the assembler.
 */
typedef struct file_analyze{
    char file_name[MAX_LEN_FILE_NAME]; /**< Name of the file being analyzed. */
    Symbol_Table *symbol_table;        /**< Symbol table of the file. */
    Data_Segment *data_segment;        /**< Data segment for the file. */
    Op_List *op_list;
    Entry_List *entry_list;
    int IC;                            /**< Instruction Counter */
    int DC;                            /**< Data Counter */
    int error;                         /**< Flag indicating if an error occurred during analysis. */
}file_analyze;

/**
 * @brief Initializes a file analysis structure.
 *
 * @param f         Pointer to the file analysis structure to initialize.
 * @param file_name Name of the file to analyze.
 * @param as_file   Pointer to the input file (file.as)
 * @param am_file   Pointer to the output of the pre-assembler file (file.am)
 * @return          TRUE if initialization succeeds, FALSE otherwise.
 */
Bool init_file_analyze (file_analyze *f, char *file_name,
                             FILE **as_file, FILE **am_file);

/**
 * @brief Frees resources associated with a file analysis structure.
 *
 * @param f Pointer to the file analysis structure to free.
 */
void free_file_analyze (file_analyze *f);

/**
 * @brief Opens a file with the specified mode and extension.
 *
 * @param file_name Name of the file to open.
 * @param extension File extension to append to the file name
 *                  (e.g., ".as", ".am", ".ob", ".ent", ".ext")
 * @param mode      File mode for opening (e.g., "r", "w", "w+").
 * @return          Pointer to the opened file stream if successful,
 *                  NULL otherwise.
 */
FILE* open_file(char* file_name, const char *extension, const char *mode);

/**
 * @brief Removes a file with the specified extension.
 *
 * @param file_name Name of the file to remove.
 * @param extension File extension to append to the file name.
 *                  (e.g., ".as", ".am", ".ob", ".ent", ".ext")
 */
void remove_file(char* file_name, const char *extension);

#endif /* _FILE_STRUCTURES_H_ */

