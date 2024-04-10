/**
 * @brief Implementation of finite state machine (FSM) functions for syntax and
 *        semantic analysis.
 *
 * The FSM is responsible for parsing and validating each line of the code,
 * performing syntax analysis, and handling part of the semantic analysis, such
 * as checking for valid addressing modes and preventing redeclaration of labels.
 *
 */


#ifndef _FSM_H_
#define _FSM_H_

#include "setting.h"
#include "utils/text.h"
#include "utils/errors.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/dataSeg.h"
#include "fileStructures/codeSeg.h"
#include "analysis.h"

/**
 * @brief Runs the finite state machine (FSM) to perform syntax and
 * semantic analysis on the provided line of assembly code.
 *
 * @param line          Pointer to the LineInfo structure containing information about the line.
 * @param symbol_table  Pointer to the symbol table.
 * @return              TRUE if syntax analysis is successful, FALSE otherwise.
 *
 * @note
 * Before passing a line to the FSM, ensure LineInfo is appropriately set up:
 *
 * - Set the line type correctly set
 *    (e.g., str_line / op_line / ent_ext_line / data_line / define_line )
 *
 * - Place the first word after the word that determines the line's type in
 *  the token buffer. For example:
 *  CODE: mov STR , STR[2]
 *            ~~~
 *
 * - Make sure LineInfo contains any necessary buffers for FSM analysis.
 */
Bool run_fsm (LineInfo *line, Symbol_Table *symbol_table);

#endif /* _FSM_H_ */
