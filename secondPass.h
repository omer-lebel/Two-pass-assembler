/**
 * @file secondPass.h
 * @brief Implementation of the second pass of the assembler.
 *
 * This file contains the implementation of the second pass of the assembler,
 * where the code generated in the first pass is further analyzed to produce
 * final machine code and resolve symbol references.
 *
 * If no errors occur, the second pass generates the following output files:
 * - fileName.ob - machine code translation of the assembly code.
 * - fileName.ent - list of entry symbols and their memory addresses.
 * - fileName.ext - list of external symbol usage and memory locations for
 *   linker resolution.
 *
 * Enable DEBUG in settings.h to see additional debugging output.
 */

#ifndef _SECOND_PASS_H_
#define _SECOND_PASS_H_

#include "setting.h"
#include "fileStructures/fileStructures.h"


/**
 * @brief Performs the second pass analysis of the assembler code.
 *
 * This function is responsible for conducting the second pass analysis of the assembler code.
 * It checks for unresolved symbols and entries, printing corresponding errors if necessary.
 * If there are no unresolved symbols or entries, and no other errors are detected,
 * it updates the data symbol addresses and generates the final analysis
 * report (files .ob, .ent, .ext)
 *
 * @param f Pointer to the file analysis structure.
 * @return An exit_code representing the result of the second pass analysis:
 *         - SUCCESS if the analysis is successful and no errors are found.
 *         - ERROR if an error occurs during the analysis.
 *         - MEMORY_ERROR if a memory allocation error occurs during processing.
 */
exit_code secondPass (file_analyze *f);



#endif /* _SECOND_PASS_H_ */
