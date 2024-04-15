/**
 * @file firstAnalysis.h
 * @brief Header file containing functions for analyzing assembly code
 * during the first pass.
 *
 * It includes functions for:
 * 1) setting line type and buffer in the LineInfo struct, and than calling fsm
 *    for syntax analysis
 * 2) updating symbol table (adding new symbol or resolving old symbols)
 * 3) adding the line data to suitable data structures that are saved for
 *    the second pass: data_segment, opLinesList, or entryLinesList.
 */

#ifndef _FIRST_PASS_ANALYSIS_H_
#define _FIRST_PASS_ANALYSIS_H_

/* ------------------------------- includes ------------------------------- */
#include "setting.h"
#include "analysis.h"
#include "fileStructures/dataSeg.h"
#include "fileStructures/codeSeg.h"
#include "fileStructures/entryLines.h"
/* ------------------------------------------------------------------------- */

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
 * @return exit_code
 */
exit_code str_analyze (LineInfo *line, Symbol_Table *symbol_table,
                       DataSegment *data_segment, int *DC);

/**
 * @brief Analyzes a data declaration line.
 *
 * This function analyzes a data directive line in the assembly code,
 * updates the symbol table, and adds the data to the data segment.
 *
 * @param line          The line to analyze.
 * @param symbol_table  The symbol table to update.
 * @param data_segment  The data segment to update.
 * @param DC            The data counter to update.
 * @return exit_code
 */
exit_code data_analyze (LineInfo *line, Symbol_Table *symbol_table,
                        DataSegment *data_segment, int *DC);

/**
 * @brief Analyzes an instruction line.
 *
 * This function analyzes an instruction line in the assembly code
 * and updates the symbol table, the instruction counter (IC) and add the
 * line to the operator line (OpLine) list
 *
 * @param line          The line to analyze.
 * @param opcode        The opcode to analyze.
 * @param f             The file being analyzed.
 * @return exit_code
 */
exit_code op_analyze (LineInfo *line, Opcode opcode, Symbol_Table *symbol_table,
                      OpLinesList *op_list, int *IC);

/**
 * @brief Analyzes a define directive line.
 *
 * This function analyzes a define directive line in the assembly code
 * and updates the symbol table accordingly.
 *
 * @param line          The line to analyze.
 * @param symbol_table  The symbol table to update.
 * @return exit_code
 */
exit_code define_analyze (LineInfo *line, Symbol_Table *symbol_table);

/**
 * @brief Analyzes an entry directive line.
 *
 * This function analyzes an entry directive line in the assembly code
 * and updates the symbol table and entry list accordingly.
 *
 * @param line          The line to analyze.
 * @param symbol_table  The symbol table to update.
 * @param entry_list    The entry list to update.
 * @return exit_code
 */
exit_code ent_analyze (LineInfo *line, Symbol_Table *symbol_table,
                       EntryLinesList *entry_list);


/**
 * @brief Analyzes an extern directive line.
 *
 * This function analyzes an extern directive line in the assembly code
 * and updates the symbol table accordingly.
 *
 * @param line          The line to analyze.
 * @param symbol_table  The symbol table to update.
 * @return exit_code
 */
exit_code ext_analyze     (LineInfo *line, Symbol_Table *symbol_table);


/**
 * @brief Analyzes an unrecognized line in the assembly code.
 *
 * This function handles analysis for lines that do not match any recognized
 * pattern.
 *
 * @param line  The line to analyze.
 * @return exit_code
 */
exit_code else_analyze (LineInfo *line);

#endif /* _FIRST_PASS_ANALYSIS_H_ */
