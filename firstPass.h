/**
 * @file firstPass.h
 * @brief Implementation of the first pass of the assembler.
 *
 * This file contains the implementation of the first pass of the assembler,
 * where each line of the code is examined for syntax and semantics analysis.
 *
 * The first pass produces several data structures that are used for further
 * analysis in the second pass:
 * - SymbolTable: Stores information about symbols encountered in the code.
 * - IC and DC: update the data and instruction counter <br>
 * - DateSegment: Stores data declarations encountered in the code. <br>
 * - OpLinesList: A list holding all instruction lines, each analyzed. <br>
 * - EntryLinesList: A list holding lines declaring entry symbols.
 * Enable DEBUG in settings.h to display these in the output.
 *
 * @note Forward declaration resolution is supported, allowing symbols to be
 * referenced before they are declared. Therefore, further analysis of
 * symbol usage occurs in the second pass.
 */

#include "fileStructures/fileStructures.h"

#ifndef _FIRST_PASS_H_
#define _FIRST_PASS_H_

/**
 * @brief Performs the first pass analysis of the assembler code.
 *
 *
 *
 * @param input_file the file contain the assembly source code to analyze
 * @param f Pointer to the file analysis structure.
 * @return An exit_code representing the result of the pre assembler analysis:
 *         - SUCCESS if the analysis is successful and no errors are found.
 *         - ERROR if an error occurs during the analysis.
 *         - MEMORY_ERROR if a memory allocation error occurs during processing.
 *
 * @note Every symbol encountered during the analysis is inserted into the symbol
 * table, and it may be marked as an unresolved symbol initially. During
 * the pass, symbols may be updated to resolved status. <br>
 */
exit_code firstPass (FILE *input_file, file_analyze* f);


#endif /*_FIRST_PASS_H_ */
