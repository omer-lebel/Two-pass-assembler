/**
 * @file preAssembler.h
 * @brief Implementation of the pre processor of the assembler.
 *
 * This file contains the implementation of the preprocessor for the assembler,
 * which preprocesses the input file and generates the .am by performing
 * the following tasks:
 * - Expanding macros <br>
 * - Replacing comment lines with empty lines <br>
 *
 * @Assumptions
 * - No nested macro settings are supported. <br>
 * - Macro are used as a single word in a line. <br>
 * - Comments are always occupy the entire line, identifiable by the first word
 */

#ifndef _PRE_ASSEMBLER_H_
#define _PRE_ASSEMBLER_H_

#include "setting.h"
#include "fileStructures/fileStructures.h"


/**
* @brief Performs the Preprocesses pass analysis of the assembler code.
*
 * Preprocesses the input file by expanding macros and replacing comment
 * lines with empty lines
*
 * @param file_name     The name of rhe input file to be preprocessed.
 * @param input         The input file to be preprocessed.
 * @param output        The output file to store the preprocessed content.
 * @param error         Pointer to the error counter.
 * @return An exit_code representing the result of the pre assembler analysis:
 *         - SUCCESS if the analysis is successful and no errors are found.
 *         - ERROR if an error occurs during the analysis.
 *         - MEMORY_ERROR if a memory allocation error occurs during processing.
*/
exit_code preAssembler (char *file_name, FILE *input, FILE *output, int* error);



#endif /*_PRE_ASSEMBLER_H_ */

