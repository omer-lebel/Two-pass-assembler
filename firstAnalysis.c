/* ------------------------------- includes ------------------------------- */
#include "firstAnalysis.h"
#include "fileStructures/symbolTable.h"
#include "utils/text.h"
#include "utils/errors.h"
#include "fsm.h"
/* ---------------------- helper function declaration ---------------------- */

/* generic analyze function */
exit_code data_n_str_analyze (LineInfo *line, Symbol_Table *symbol_table, int DC);
exit_code ext_ent_analyze (LineInfo *line, Symbol_Table *symbol_table);

/* managing labels */
exit_code manage_data_label (LineInfo *line, Symbol_Table *symbol_table, int DC);
exit_code manage_define_const (LineInfo *line, Symbol_Table *symbol_table);
exit_code manage_extern_label (LineInfo *line, Symbol_Table *symbol_table);
exit_code manage_entry_label (LineInfo *line, Symbol_Table *symbol_table, EntryLinesList *entry_list);
exit_code manage_op_labels (LineInfo *line, Symbol_Table *symbol_table);
Symbol* manage_code_label (Symbol_Table *symbol_table, char *label, int address);
Bool manage_operands_labels (Symbol_Table *symbol_table, Symbol *label, Operand *src, Operand *target);
Symbol *manage_new_label_usage (Symbol_Table *symbol_table, Operand *operand, Symbol *label);
void update_unresolved_count (Symbol_Data *symbol_data, Symbol_Table *symbol_table);
/* ------------------------------------------------------------------------- */


/* =========================================================================
 *                            data and string
 * ========================================================================= */

exit_code str_analyze (LineInfo *line, Symbol_Table *symbol_table,
                       DataSegment *data_segment, int *DC)
{
  exit_code res;
  char str_content[MAX_STR_LENGTH] = "";
  line->type = str_line;
  line->info.str.content = str_content;

  res = data_n_str_analyze (line, symbol_table, *DC);
  if (res != SUCCESS) {
    return res;
  }

  /* add to data seg */
  if (!add_to_data_segment (data_segment, DC, CHAR_TYPE, line->info.str.content,
                            line->info.str.len + 1)) {
    return MEMORY_ERROR;
  }

  return SUCCESS;
}

exit_code data_analyze (LineInfo *line, Symbol_Table *symbol_table,
                        DataSegment *data_segment, int *DC)
{
  int data_arr[MAX_DATA_ARR_LENGTH];
  exit_code res;
  line->type = data_line;
  line->info.data.arr = data_arr;

  res = data_n_str_analyze (line, symbol_table, *DC);
  if (res != SUCCESS) {
    return res;
  }

  /* add to data seg */
  if (!add_to_data_segment (data_segment, DC, INT_TYPE, line->info.data.arr,
                            line->info.data.len)) {
    return MEMORY_ERROR;
  }

  return SUCCESS;
}

/**
 * @brief Analyzes a line containing either data or string directive during
 * the first pass.
 *
  * This function analyzes the line and updates the symbol table and the data
  * segment accordingly.
 *
 * @param line          The line to analyze.
 * @param symbol_table  The symbol table.
 * @param DC            The data counter.
 * @return exit code
 */
exit_code data_n_str_analyze (LineInfo *line, Symbol_Table *symbol_table,
                              int DC)
{
  if (!run_fsm (line, symbol_table)) {
    return ERROR;
  }

  if (IS_EMPTY(line->label)) { /* .string "a" */
    raise_warning (inaccessible_data, line->parts);
    return SUCCESS;
  }
  /* else, add or update symbol table */
  return manage_data_label (line, symbol_table, DC);

}

/**
 * @brief Manages the usage of a data label (before .data or .string) in the
 * symbol table.
 *
 * Managing either by adding a new data symbol to the symbol table or resolving
 * an existing one. <br>
 * If the symbol already exists, it updates the unresolved count and updates
 * the symbol fields.
 *
 * @param line The line containing the data label.
 * @param symbol_table The symbol table.
 * @param DC The data counter.
 * @return exit code
 */
exit_code manage_data_label (LineInfo *line, Symbol_Table *symbol_table, int DC)

{
  Symbol_Data *symbol_data;
  Symbol *symbol = find_symbol (symbol_table, line->label);
  if (!symbol) {
    if (!add_symbol (symbol_table, line->label, DATA, DC, NOT_ENTRY)) {
      return MEMORY_ERROR;
    }
    return SUCCESS;
  }
  /* else, the symbol already exist.
   * since validation already done, it's must be unresolved symbol */
  symbol_data = (Symbol_Data *) symbol->data;
  update_unresolved_count (symbol_data, symbol_table);
  symbol_data->type = DATA;
  symbol_data->val = DC;
  return SUCCESS;
}


/* =========================================================================
 *                                 operator
 * ========================================================================= */
exit_code op_analyze (LineInfo *line, Opcode opcode, Symbol_Table
*symbol_table, OpLinesList *op_list, int *IC)
{
  char src_sym_buffer[MAX_LINE_LEN];
  char target_sym_buffer[MAX_LINE_LEN];
  OpAnalyze op;
  init_op_analyze (&op, opcode, src_sym_buffer, target_sym_buffer);
  line->info.op = &op;
  line->type = op_line;

  if (!run_fsm (line, symbol_table)) {
    return ERROR;
  }

  line->info.op->address = *IC;
  if (manage_op_labels (line, symbol_table) != SUCCESS) {
    return MEMORY_ERROR;
  }

  if (!add_to_op_lines_list (op_list, line->info.op, line->parts)) {
    return MEMORY_ERROR;
  }

  *IC += calc_op_size (line->info.op);
  return SUCCESS;
}


/**
 * @brief Processes labels of an instruction line.
 *
 * This function processes all labels associated with an instruction line,
 * including the label at the beginning of the line (code label) and labels
 * used in the source and target operands.
 *
 * @param line          The instruction line to process.
 * @param symbol_table  The symbol table to update.
 * @return exit code
 */
exit_code manage_op_labels (LineInfo *line, Symbol_Table *symbol_table)
{
  Symbol *label = NULL;
  Operand *src = &line->info.op->src;
  Operand *target = &line->info.op->target;

  /* process code symbol (label at the beginning of the line) */
  if (!IS_EMPTY(line->label)) {
    label = manage_code_label (symbol_table, line->label, line->info.op->address);
    if (!label) {
      return MEMORY_ERROR;
    }
  }
  if (!manage_operands_labels (symbol_table, label, src, target)){
    return MEMORY_ERROR;
  }
  return SUCCESS;

}

/**
 * @brief Manages the processing of symbols that used within operands in an
 * instruction line.
 *
 * This function updates the symbol table as necessary for labels used in
 * the source and target operands.
 * If a symbol does not exist in the symbol table, it is added as an
 * unresolved symbol.
 *
 * @param symbol_table  The symbol table to update.
 * @param label         The label associated with the instruction line.
 * @param src           The source operand to process.
 * @param target        The target operand to process.
 * @return A boolean value indicating success or memory failure.
 */
Bool
manage_operands_labels (Symbol_Table *symbol_table, Symbol *label,
                        Operand *src, Operand *target)
{
  /* note that operand->info.symInx.found is set to true for all the add
   * mode by default, so only unresolved symbol will be checked here */

  Symbol *symbol;
  /* src and target share the same unresolved symbol */
  if (!src->info.symInx.found && !target->info.symInx.found
      && strcmp (src->info.symInx.symbol, target->info.symInx.symbol) == 0) {
    if (!(symbol = manage_new_label_usage (symbol_table, src, label))) {
      return FALSE;
    }
    target->info.symInx.symbol = symbol;
    target->info.symInx.found = TRUE;
    return TRUE;
  }
  /* src use unresolved label */
  if (!src->info.symInx.found) {
    if (!manage_new_label_usage (symbol_table, src, label)) {
      return FALSE;
    }
  }
  /* target use unresolved label */
  if (!target->info.symInx.found) {
    if (!manage_new_label_usage (symbol_table, target, label)) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * @brief Processes a new label usage within an operand
 *
 * If the label is not exist yet in the symbol table, the function insert it
 * and mark it as unresolved symbol
 *
 * @param symbol_table  The symbol table.
 * @param operand       The operand to process.
 * @param label         The label associated with the operand.
 * @return A pointer to the symbol added to the symbol table.
 */
Symbol *manage_new_label_usage (Symbol_Table *symbol_table, Operand *operand,
                                Symbol *label)
{
  Symbol *symbol;
  /* operand use the label that declare at the start of the line */
  if (label && strcmp (operand->info.symInx.symbol, label->word) == 0) {
    symbol = label;
  }
  else {
    symbol = add_symbol (symbol_table, operand->info.symInx.symbol,
                         UNRESOLVED_USAGE, 0, NOT_ENTRY);
    if (!symbol) {
      return NULL;
    }
  }
  operand->info.symInx.symbol = symbol;
  operand->info.symInx.found = TRUE;
  symbol_table->unresolved_symbols_count++;
  return symbol;
}

/**
 * @brief Managing a code label (the label at the beginning of the line) of
 * an instruction line.
 *
 * Managing either by adding a new code symbol to the symbol table or resolving
 * an existing one. <br>
 * If the symbol already exists, it updates the unresolved count and updates
 * the symbol fields.
 *
 * @param symbol_table  The symbol table to update.
 * @param label         The label to process.
 * @param address       The address associated with the label.
 * @return Pointer to the symbol associated with the code label.
 */

Symbol *
manage_code_label (Symbol_Table *symbol_table, char *label, int address)
{
  Symbol_Data *symbol_data;
  Symbol *symbol = find_symbol (symbol_table, label);
  if (!symbol) {
    return add_symbol (symbol_table, label, CODE, address, NOT_ENTRY);
  }
  /* else, the symbol already exist.
   * since validation already done, it's must be unresolved symbol */
  symbol_data = (Symbol_Data *) symbol->data;
  update_unresolved_count (symbol_data, symbol_table);
  symbol_data->type = CODE;
  symbol_data->val = address;
  return symbol;
}

/**
 * @brief Updates the unresolved symbol count when a symbol is resolved.
 *
 * @param symbol_data The data associated with the resolved symbol.
 * @param symbol_table The symbol table to update.
 */
void
update_unresolved_count (Symbol_Data *symbol_data, Symbol_Table *symbol_table)
{
  if (symbol_data->type == UNRESOLVED_ENTRY_USAGE) {
    symbol_table->unresolved_entry_count--;
    symbol_table->unresolved_symbols_count--;
  }
  else if (symbol_data->type == UNRESOLVED_USAGE) {
    symbol_table->unresolved_symbols_count--;
  }
  else { /* UNRESOLVED_ENTRY_USAGE */
    symbol_table->unresolved_entry_count--;
  }
}


/* =========================================================================
 *                                define
 * ========================================================================= */
exit_code define_analyze (LineInfo *line, Symbol_Table *symbol_table)
{
  char def_name[MAX_LABEL_LEN] = "";

  /* label and define at the same line */
  if (!IS_EMPTY(line->label)) { /* LABEL: .define x=3 */
    raise_error (label_and_define_on_same_line_err, line->parts);
    return ERROR;
  }

  line->type = def_line;
  line->info.define.name = def_name;
  if (!run_fsm (line, symbol_table)) {
    return ERROR;
  }

  return manage_define_const (line, symbol_table);
}

/**
 * @brief Adds a define constant to the symbol table
 *
 * This function adds a new define constant to the symbol table.<br>
 * If the constant is already in the symbol table: if it has the same
 * value - warning, otherwise - error.
 *
 * @param line The line containing the .define directive.
 * @param symbol_table The symbol table to update.
 * @return  exit_code    indicating the success or memory failure
 */
exit_code manage_define_const (LineInfo *line, Symbol_Table *symbol_table)
{
  Symbol *symbol;
  Symbol_Data *symbol_data;

  if (!line->info.define.found) {
    if (!(symbol = add_symbol (symbol_table, line->info.define.name,
                               DEFINE, line->info.define.val, NOT_ENTRY))) {
      return MEMORY_ERROR;
    }
    line->info.define.name = symbol;
    return SUCCESS;
  }
  /* else, symbol already exist */
  /* only redeclaration allowed here is redeclaration of define */
  symbol = (Symbol *) line->info.define.name;
  symbol_data = (Symbol_Data *) symbol->data;
  get_identifier_tok (line->parts, FALSE);
  if (symbol_data->val != line->info.define.val) { /* x = 1 | x = 2 */
    raise_error (redeclaration_err, line->parts);
    return ERROR;
  }
  /* .define x = 1 | .define x = 1 */
  raise_warning (duplicate_declaration_warning, line->parts);
  return SUCCESS;
}

/* =========================================================================
 *                                entry and extern
 * ========================================================================= */
exit_code ent_analyze (LineInfo *line, Symbol_Table *symbol_table,
                       EntryLinesList *entry_list)
{
  char name[MAX_LABEL_LEN] = "";
  line->info.ext_ent.name = name;
  line->type = ent_line;

  if (ext_ent_analyze (line, symbol_table) == ERROR) {
    return ERROR;
  }
  return manage_entry_label (line, symbol_table, entry_list);
}

exit_code ext_analyze (LineInfo *line, Symbol_Table *symbol_table)
{
  char name[MAX_LABEL_LEN] = "";
  line->info.ext_ent.name = name;
  line->type = ext_line;

  if (ext_ent_analyze (line, symbol_table) == ERROR) {
    return ERROR;
  }
  return manage_extern_label (line, symbol_table);
}

/**
 * @brief Analyzes external or entry directives in the assembly code.
 *
 * @param line          The line to analyze.
 * @param symbol_table  The symbol table to update.
 * @return exit_code
 */
exit_code ext_ent_analyze (LineInfo *line, Symbol_Table *symbol_table)
{
  if (!run_fsm (line, symbol_table)) {
    return ERROR;
  }

  if (!IS_EMPTY(line->label)) {
    raise_warning (ignored_label, line->parts);
  }
  return SUCCESS;
}

/**
 * @brief Manages the addition of an entry label to the symbol table and for
 * the entry lines list
 *
 * If the specified entry label does not exist in the symbol table, the
 * function adds it as an unresolved entry symbol
 *
 * @param line          The line that hold that entry label
 * @param symbol_table  The symbol table.
 * @param entry_list The list of entry lines.
 * @return exit code
 */
exit_code manage_entry_label (LineInfo *line, Symbol_Table *symbol_table,
                              EntryLinesList *entry_list)
{
  Symbol_Data *symbol_data;
  Symbol *symbol;
  LineParts *resolved_flag = NULL;

  if (!line->info.ext_ent.found) { /*new symbol */
    if (!(symbol = add_symbol (symbol_table, line->info.ext_ent.name,
                               UNRESOLVED_ENTRY, 0, IS_Entry))
        || !add_to_entry_lines_list (entry_list, symbol, line->parts)) {
      return MEMORY_ERROR;
    }
    line->info.ext_ent.name = symbol;
    symbol_table->unresolved_entry_count++;
    return SUCCESS;
  }

  /* else, symbol already exist. (unresolved / entry / data / code)*/
  symbol = (Symbol *) line->info.define.name;
  symbol_data = symbol->data;

  if (!symbol_data->isEntry) {
    symbol_data->isEntry = IS_Entry;

    if (symbol_data->type == UNRESOLVED_USAGE) {
      symbol_data->type = UNRESOLVED_ENTRY_USAGE;
      symbol_table->unresolved_entry_count++;
      resolved_flag = line->parts;
    }

    if (!add_to_entry_lines_list (entry_list, symbol, resolved_flag)) {
      return MEMORY_ERROR;
    }
  }
  return SUCCESS;
}

/**
 * @brief Manages the usage of an external label in the symbol table.
 *
 * Managing either by adding a new extern symbol to the symbol table or
 * resolving an existing one. <br>
 * If the symbol already exists, it updates the unresolved count and updates
 * the symbol fields.
 *
 * @param line          The line containing the external label.
 * @param symbol_table  The symbol table.
 * @return exit code
 */
exit_code manage_extern_label (LineInfo *line, Symbol_Table *symbol_table)
{
  Symbol_Data *symbol_data;
  Symbol *symbol;

  if (!line->info.ext_ent.found) { /*new symbol */
    if (!(symbol = add_symbol (symbol_table, line->info.ext_ent.name,
                               EXTERN, 0, NOT_ENTRY))) {
      return MEMORY_ERROR;
    }
    symbol_table->extern_count++;
    line->info.ext_ent.name = symbol;
    return SUCCESS;
  }

  /* else, label already exist. can be: extern or unresolved */
  symbol = (Symbol *) line->info.define.name;
  symbol_data = symbol->data;

  if (symbol_data->type != EXTERN) { /* unresolved */
    symbol_data->type = EXTERN;
    symbol_table->unresolved_symbols_count--;
    symbol_table->extern_count++;
  }
  return SUCCESS;
}

/* =========================================================================
 *                                else
 * ========================================================================= */
exit_code else_analyze (LineInfo *line)
{
  char *first_word = line->parts->token;
  /* if it's valid symbol name, but with no ':' */
  if (IS_EMPTY(line->label)
      && is_valid_identifier (line->parts, first_word, FALSE)) {
    raise_error (expected_colon_err, line->parts);
  }
  else {
    raise_error (undefined_command_err, line->parts);
  }
  return ERROR;
}
