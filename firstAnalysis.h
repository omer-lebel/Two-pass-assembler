/**
 * @file firstAnalysis.h
 * @brief Header file containing functions for analyzing assembly code.
 *
 * 1) set line type and buffer in the Line_Info struct, and than call fsm,
 * for syntax analysis
 * 2) update symbol table (using the process label function)
 * 3) add the line data to suit data structures that being saved to te
 * second pass: data_segment, op_line or entry_line
 */

#ifndef _FIRST_PASS_ANALYSIS_H_
#define _FIRST_PASS_ANALYSIS_H_

#include "setting.h"
#include "utils/text.h"
#include "utils/errors.h"
#include "fileStructures/fileStructures.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/dataSeg.h"
#include "fileStructures/codeSeg.h"
#include "analysis.h"
#include "fsm.h"

/**
 * @brief Analyzes a string declaration line.
 *
 * This function analyzes a string directive line in the assembly code,
 * updates the symbol table, and adds the string to the data segment.
 *
 * @param line          The line to analyze.
 * @param symbol_table  The symbol table to update.
 * @param data_segment  The data segment to add the string to.
 * @param DC            The data counter to update.
 * @return              exit_code Indicating the success or failure of the analysis.
 */
exit_code str_analyze (LineInfo *line, Symbol_Table *symbol_table,
                       Data_Segment *data_segment, int *DC);

/**
 * @brief Analyzes a data declaration line.
 *
 * This function analyzes a data directive line in the assembly code,
 * updates the symbol table, and adds the data to the data segment.
 *
 * @param line The line to analyze.
 * @param symbol_table The symbol table to update.
 * @param data_segment The data segment to update.
 * @param DC The data counter to update.
 * @return exit_code Indicating the success or failure of the analysis.
 */
exit_code data_analyze    (LineInfo *line, Symbol_Table *symbol_table, Data_Segment *data_segment, int *DC);
exit_code ent_analyze     (LineInfo *line, Symbol_Table *symbol_table, Entry_List *entry_list);
exit_code define_analyze  (LineInfo *line, Symbol_Table *symbol_table);
exit_code ext_analyze     (LineInfo *line, Symbol_Table *symbol_table);
exit_code op_analyze      (LineInfo *line, Opcode opcode, file_analyze *f);

/**
 * @brief Analyzes an unrecognized line in the assembly code.
 *
 * @param line The line to analyze.
 * @return exit_code Indicating the success or failure of the analysis.
 */
exit_code else_analyze (LineInfo *line);

#endif /* _FIRST_PASS_ANALYSIS_H_ */
