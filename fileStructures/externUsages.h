/**
 * @file externUsages.h
 * @brief Managing the table of external symbols and their usages.
 *
 * It defines functions to analyze the code segment, extract information
 * about external symbols and their usage locations,
 * and then build a table representing this information. The table is then printed
 * to a specified file stream, typically an `.ext` file, to allow external linkage
 * in subsequent compilation stages.
 */

#ifndef _EXTERN_USAGES_H_
#define _EXTERN_USAGES_H_

#include "../setting.h"
#include "../utils/vector.h"
#include "../utils/linkedList.h"
#include "fileStructures.h"
#include "codeSeg.h"

/**
 * @brief Builds the table of external symbols and their usages base on the
 *        provided list of operation, and print it to a wanted output file stream.
 *
 * @param op_list    Pointer to the list of operations.
 * @param output    File stream to print the table to.
 * @param file_name Name of the file.
 * @return          - SUCCESS if ended successfully <br>
 *                  - MEMORY_ERROR if memory error cured while building the table
 */
exit_code print_extern_table(Op_List *op_list, FILE *output, char *file_name);



#endif /* _EXTERN_USAGES_H_ */
