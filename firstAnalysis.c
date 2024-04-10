
#include "firstAnalysis.h"
/* ---------------------- helper function declaration ---------------------- */
exit_code ext_ent_analyze (LineInfo *line, Symbol_Table *symbol_table);
exit_code directive_analyze (LineInfo *line, Symbol_Table *symbol_table, int DC);

/*
 * processing labels function: update symbol table by adding new symbols and
 * update unresolved symbol count
 */
Bool process_op_labels (Symbol_Table *symbol_table, LineInfo *line);
Symbol * process_code_label (Symbol_Table *symbol_table, char *label, int address);
exit_code process_extern_label (LineInfo *line, Symbol_Table *symbol_table);
exit_code process_entry_label (LineInfo *line, Symbol_Table *symbol_table, Entry_List *entry_list);
exit_code process_define (LineInfo *line, Symbol_Table *symbol_table);
Bool process_operands_labels (Symbol_Table *symbol_table, Symbol *label, Operand *src, Operand *target);
Symbol *proc_operand_label (Symbol_Table *symbol_table, Operand *operand, Symbol *label);
exit_code process_directive_label (LineInfo *line, Symbol_Table *symbol_table, int DC);
void update_unresolved_count (Symbol_Data *symbol_data, Symbol_Table *symbol_table);



/* ************************ data or string *************************/

exit_code str_analyze (LineInfo *line, Symbol_Table *symbol_table,
                       Data_Segment *data_segment, int *DC)
{
  exit_code res;
  char str_content[MAX_STR_LENGTH] = "";
  line->type = str_line;
  line->info.str.content = str_content;

  res = directive_analyze (line, symbol_table, *DC);
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
                        Data_Segment *data_segment, int *DC)
{
  int data_arr[MAX_DATA_ARR_LENGTH];
  exit_code res;
  line->type = data_line;
  line->info.data.arr = data_arr;

  res = directive_analyze (line, symbol_table, *DC);
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

exit_code directive_analyze (LineInfo *line, Symbol_Table *symbol_table,
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
  return process_directive_label (line, symbol_table, DC);

}


/**
 * @brief Processes the directive label.
 *
 * Processes the directive label and updates the symbol table accordingly.
 *
 * @param line The line to analyze.
 * @param symbol_table The symbol table to update.
 * @param DC The data counter.
 * @return exit_code Indicating the success or failure of the processing.
 */
exit_code process_directive_label (LineInfo *line, Symbol_Table *symbol_table,
                                   int DC)
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

/************************* operator *************************/
exit_code op_analyze (LineInfo *line, Opcode opcode, file_analyze *f)
{
  char src_sym_buffer[MAX_LINE_LEN];
  char target_sym_buffer[MAX_LINE_LEN];
  Op_Analyze op;
  init_op_analyze (&op, opcode, src_sym_buffer, target_sym_buffer);
  line->info.op = &op;
  line->type = op_line;

  if (!run_fsm (line, f->symbol_table)) {
    return ERROR;
  }

  line->info.op->address = f->IC;
  if (!process_op_labels (f->symbol_table, line)) {
    return MEMORY_ERROR;
  }

  if (!add_to_op_list (f->op_list, line->info.op, line->parts)) {
    return MEMORY_ERROR;
  }

  f->IC += calc_op_size (line->info.op);
  return SUCCESS;
}

Bool process_op_labels (Symbol_Table *symbol_table, LineInfo *line)
{
  Symbol *label = NULL;
  Operand *src = &line->info.op->src, *target = &line->info.op->target;

  /* process code symbol (label at the beginning of the line) */
  if (!IS_EMPTY(line->label)) {
    label = process_code_label (symbol_table, line->label, line->info.op->address);
    if (!label) {
      return FALSE;
    }
  }
  return process_operands_labels (symbol_table, label, src, target);

}
Bool
process_operands_labels (Symbol_Table *symbol_table, Symbol *label, Operand *src, Operand *target)
{
  /* note that operand->info.symInx.found is set to true for all the add
   * mode by default, so only unresolved symbol will be check here */

  Symbol *symbol;
  /* src and target share the same unresolved symbol */
  if (!src->info.symInx.found && !target->info.symInx.found
      && strcmp (src->info.symInx.symbol, target->info.symInx.symbol) == 0) {
    if (!(symbol = proc_operand_label (symbol_table, src, label))) {
      return FALSE;
    }
    target->info.symInx.symbol = symbol;
    target->info.symInx.found = TRUE;
    return TRUE;
  }
  /* src use unresolved label */
  if (!src->info.symInx.found) {
    if (!proc_operand_label (symbol_table, src, label)) {
      return FALSE;
    }
  }
  /* target use unresolved label */
  if (!target->info.symInx.found) {
    if (!proc_operand_label (symbol_table, target, label)) {
      return FALSE;
    }
  }
  return TRUE;
}

Symbol *proc_operand_label (Symbol_Table *symbol_table, Operand *operand,
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

Symbol *
process_code_label (Symbol_Table *symbol_table, char *label, int address)
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

/************************* define *************************/
exit_code define_analyze (LineInfo *line, Symbol_Table *symbol_table)
{
  char def_name[MAX_LINE_LEN] = "";

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

  return process_define (line, symbol_table);
}

exit_code process_define (LineInfo *line, Symbol_Table *symbol_table)
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

/************************* ent ext *************************/
exit_code ent_analyze (LineInfo *line, Symbol_Table *symbol_table,
                       Entry_List *entry_list)
{
  char name[MAX_LINE_LEN] = "";
  line->info.ext_ent.name = name;
  line->type = ent_line;

  if (ext_ent_analyze (line, symbol_table) == ERROR) {
    return ERROR;
  }
  return process_entry_label (line, symbol_table, entry_list);
}

exit_code ext_analyze (LineInfo *line, Symbol_Table *symbol_table)
{
  char name[MAX_LINE_LEN] = "";
  line->info.ext_ent.name = name;
  line->type = ext_line;

  if (ext_ent_analyze (line, symbol_table) == ERROR) {
    return ERROR;
  }
  return process_extern_label (line, symbol_table);
}

/**
 * @brief Analyzes external and entry directives in the assembly code.
 *
 * @param line The line to analyze.
 * @param symbol_table The symbol table to update.
 * @return exit_code Indicating the success or failure of the analysis.
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

exit_code process_entry_label (LineInfo *line, Symbol_Table *symbol_table,
                               Entry_List *entry_list)
{
  Symbol_Data *symbol_data;
  Symbol *symbol;
  LineParts *resolved_flag = NULL;

  if (!line->info.ext_ent.found) { /*new symbol */
    if (!(symbol = add_symbol (symbol_table, line->info.ext_ent.name,
                               UNRESOLVED_ENTRY, 0, IS_Entry))
        || !add_to_entry_list (entry_list, symbol, line->parts)) {
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

    if (!add_to_entry_list (entry_list, symbol, resolved_flag)) {
      return MEMORY_ERROR;
    }
  }
  return SUCCESS;
}

exit_code process_extern_label (LineInfo *line, Symbol_Table *symbol_table)
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

/**** else */
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