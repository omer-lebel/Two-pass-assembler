/**
 * The preAssembler program
 * preprocesses the input file by performing the following tasks: \n
 * 1) Expanding macros \n
 * 2) Removing empty lines \n
 * 3) Removing comment lines \n
 *
 * @Assumptions
 * - No nested macro settings. \n
 * - Each macro line has a closure with 'endmcr'. \n
 * - A macro is used as a single word in a line. \n
 * - Comments are always the entire line, identifiable by the first word. \n
 * - No extra text after a macro declaration (e.g., "mcr mcr_name"). \n
 * - No extra text after a macro declaration is finished (e.g., "endmcr").
 */


#ifndef _PRE_ASSEMBLER_H_
#define _PRE_ASSEMBLER_H_

#include <stdlib.h>
#include "setting.h"
#include "utils/text.h"
#include "utils/linkedList.h"

/**
 * Data structure representing the content of a macro in the linked list.
 */
typedef struct mcrData
{
    char *content;
    size_t total;
    size_t capacity;
} mcrData;


/**
 * Preprocesses the input file by expanding macros and removing empty lines
 * and comment lines
 *
 * @param input The input file to be preprocessed.
 * @param output The output file to store the preprocessed content.
 * @return Returns EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int preAssembler (FILE *input, FILE *output);

/**
 * Processes a line of the input file.
 *
 * @param output The output file to store the processed content.
 * @param mcr_list The linked list containing macro definitions.
 * @param line The current line being processed.
 * @param line_num The line number of the current line.
 * @param first_word The first word of the line.
 *  @param curr_mcr Pointer to current macro if inside a definition, otherwise NULL.
 * @return Returns TRUE on success, FALSE on failure.
 */
int p_processLine (FILE *output, LinkedList *mcr_list, char *line,
                   int line_num, char* first_word, Node **curr_mcr);

/**
 * Adds a macro definition to the macro's linked list.
 *
 * @param mcr_list The linked list containing macro definitions.
 * @param mcr_name The name of the macro to be added.
 * @return Returns a pointer to the added node on success, NULL on failure.
 */
Node *addMcr (LinkedList *mcr_list, char *mcr_name);

/**
 * Checks the validity of a macro name.
 *
 * @param mcr_list The linked list containing macro definitions.
 * @param mcr_name The name of the macro to be checked.
 * @return Returns TRUE if the macro name is valid, FALSE otherwise.
 * @note Valid macro names:
 * - Must have a name (only 'mcr' is not enough). \n
 * - Cannot be a reserved word of the language. \n
 * - Can contain only letters, numbers, and '_'. \n
 */
int isValidMcr (LinkedList *mcr_list, char *mcr_name);

/**
 * Writes a line to the output file, handling macro expansions.
 *
 * @param output The output file to store the written content.
 * @param line The current line being processed.
 * @param mcr_list The linked list containing macro definitions.
 * @param first_word The first word of the line.
 */
void p_writeLine (FILE *output, char *line, LinkedList *mcr_list,
                  char *first_word);


/* *************************************************
* ...Function to handle the macro's linked list ...
***************************************************/

/**
 * Initializes macro data structure.
 * The macro will contain it name and no content
 *
 * @param data Unused parameter.
 * @return Returns a pointer to the initialized macro data structure.
 */
void *init_mcrData (const void *data);

/**
 * Adds content of the macro to the macro data structure.
 *
 * @param mcr_data pointer to the macro data structure to add content to.
 * @param content The content to be added.
 * @return Returns TRUE on success, FALSE on failure.
 */
int add_content (mcrData *mcr_data, char *content);

/**
 * Prints macro data structure information.
 * Outputs the macro name followed by its content.
 *
 * @param macro_name The name of the macro.
 * @param macro_data Pointer to the macro data structure containing the content.
 * @param pf The file pointer for printing.
 */
void print_mcrData (const char *word, const void *data, FILE *pf);

/**
 * Frees memory allocated for the macro data structure.
 *
 * @param data The macro data structure to be freed.
 */
void free_mcrData (void *data);





#endif /*_PRE_ASSEMBLER_H_ */

