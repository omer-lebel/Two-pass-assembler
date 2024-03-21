/*
 Created by OMER on 1/15/2024.
*/

#include "firstPass.h"

#define IS_LABEL(str) (str[strlen (str) - 1] == ':')



/************************* pass helpers *************************/

/*todo change */
void free_file_analyze1 (file_analyze *f)
{
  free_symbol_table (f->symbol_table);
  free_op_list (f->op_list);
  free_data_segment (f->data_segment);
  free_entry_list (f->entry_list);
  memset (f, 0, sizeof (file_analyze));
}

exit_code init_first_pass (file_analyze *f, char *file_name,
                           LineInfo *line_info, LinePart *line_part,
                           char *label)
{
  f->symbol_table = new_symbol_table ();
  f->data_segment = new_data_segment (&f->DC);
  f->op_list = new_op_list ();
  f->entry_list = new_entry_list();
  if (!f->symbol_table || !f->data_segment || !f->op_list || !f->entry_list) {
    free_file_analyze1 (f);
    return MEMORY_ERROR;
  }

  /* line info */
  line_info->label = label;
  line_info->parts = line_part;

  line_part->file = file_name; /*todo delete */
  return SUCCESS;
}

void restart_line_info (LineInfo *line)
{
  LinePart *tmp_part = line->parts;
  char *tmp_label = line->label;

  memset (line, 0, sizeof (LineInfo));
  line->parts = tmp_part;
  line->label = tmp_label;

  restart_line_parts (line->parts);
  NULL_TERMINATE(line->label, 0);
  trim_end (line->parts->postfix);
}

/* called when symbol is resolved */
void
update_unresolved_count (Symbol_Data *symbol_data, Symbol_Table *symbol_table)
{
  if (symbol_data->type == UNRESOLVED_ENTRY_USAGE) {
    symbol_table->unresolved_entry_count--;
    symbol_table->unresolved_usage_count--;
  }
  else if (symbol_data->type == UNRESOLVED_USAGE) {
    symbol_table->unresolved_usage_count--;
  }
  else {
    symbol_table->unresolved_entry_count--;
  }
}

/************************* data or string *************************/
exit_code process_directive_label (LineInfo *line, Symbol_Table *symbol_table,
                                   int DC)
{
  Symbol_Data *symbol_data;
  Symbol_N *symbol = find_symbol (symbol_table, line->label);
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

Bool directive_analyze (LineInfo *line, Symbol_Table *symbol_table)
{
  if (!run_fsm (line, symbol_table)) {
    return FALSE;
  }

  if (IS_EMPTY(line->label)) { /* .string "a" */
    lineToPostfix (line->parts); /*get the first tok again for the error msg */
    r_warning ("", line->parts, "variables may be inaccessible without label");
    return TRUE;
  }

  return TRUE;
}
/************************* string *************************/
exit_code str_analyze (LineInfo *line, file_analyze *f)
{
  exit_code res;
  char str_content[MAX_STR_LENGTH] = "";
  line->type_l = str_l;
  line->info.str.content = str_content;

  if (!directive_analyze (line, f->symbol_table)) {
    return ERROR;
  }
  /* add or update symbol table */
  res = process_directive_label (line, f->symbol_table, f->DC);
  if (res != SUCCESS) {
    return res;
  }
  /* add to data seg */
  if (!add_to_data_segment (f->data_segment, &(f->DC), CHAR_TYPE,
                            line->info.str.content, line->info.str.len + 1)) {
    return MEMORY_ERROR;
  }

  return SUCCESS;
}

/************************* data *************************/
exit_code data_analyze (LineInfo *line, file_analyze *f)
{
  int data_arr[MAX_DATA_ARR_LENGTH];
  exit_code res;
  line->type_l = data_l;
  line->info.data.arr = data_arr;

  if (!directive_analyze (line, f->symbol_table)) {
    return ERROR;
  }
  /* add or update symbol table */
  res = process_directive_label (line, f->symbol_table, f->DC);
  if (res != SUCCESS) {
    return res;
  }

  /* add to data seg */
  if (!add_to_data_segment (f->data_segment, &(f->DC),
                            INT_TYPE, line->info.data.arr, line->info.data.len)) {
    return MEMORY_ERROR;
  }

  return SUCCESS;
}

/************************* define *************************/
exit_code process_define_label (LineInfo *line, Symbol_Table *symbol_table)
{
  Symbol_N *symbol;
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
  symbol = (Symbol_N *) line->info.define.name;
  symbol_data = (Symbol_Data*) symbol->data;
  get_identifier_tok (line->parts, FALSE);
  if (symbol_data->val != line->info.define.val) { /* x = 1 | x = 2 */
    r_error ("redeclaration of ", line->parts, "");
    return ERROR;
  }
  /* x = 1 | x = 1 */
  r_warning ("", line->parts, " has already declared in earlier line");
  return SUCCESS;
}

exit_code define_analyze (LineInfo *line, Symbol_Table *symbol_table)
{
  char def_name[MAX_LINE_LEN] = "";

  /* label and define at the same line */
  if (!IS_EMPTY(line->label)) { /* LABEL: .define x=3 */
    lineToPostfix (line->parts); /*get the fist tok again for the error msg */
    lineTok (line->parts);
    r_error ("label ", line->parts, " and '.define' cannot be declared on the"
                                    " same line");
    return ERROR;
  }

  line->type_l = def_l;
  line->info.define.name = def_name;
  if (!run_fsm (line, symbol_table)) {
    return ERROR;
  }

  return process_define_label (line, symbol_table);
}

/************************* ent ext *************************/
exit_code process_entry_label (LineInfo *line, Symbol_Table *symbol_table,
                               Entry_List *entry_list)
{
  Symbol_Data *symbol_data;
  Symbol_N *symbol;
  LinePart *resolved_flag = NULL;

  if (!line->info.ext_ent.found) { /*new symbol */
    if (!(symbol = add_symbol (symbol_table, line->info.ext_ent.name,
                      UNRESOLVED_ENTRY, 0, IS_Entry))
          || !add_to_entry_list (entry_list, symbol, resolved_flag)) {
      return MEMORY_ERROR;
    }
    line->info.ext_ent.name = symbol;
    symbol_table->unresolved_entry_count++;
    symbol_table->entry_count++;
    return SUCCESS;
  }

  /* else, symbol already exist. (unresolved / entry / data / code)*/
  symbol = (Symbol_N *) line->info.define.name;
  symbol_data = symbol->data;

  if (!symbol_data->isEntry) {
    symbol_data->isEntry = IS_Entry;
    symbol_table->entry_count++;

    if (symbol_data->type == UNRESOLVED_USAGE) {
      symbol_data->type = UNRESOLVED_ENTRY_USAGE;
      symbol_table->unresolved_entry_count++;
      resolved_flag = line->parts;
    }

    if (!add_to_entry_list (entry_list, symbol, resolved_flag)){
      return MEMORY_ERROR;
    }
  }
  return SUCCESS;
}

exit_code process_extern_label (LineInfo *line, Symbol_Table *symbol_table)
{
  Symbol_Data *symbol_data;
  Symbol_N *symbol;

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
  symbol = (Symbol_N *) line->info.define.name;
  symbol_data = symbol->data;

  if (symbol_data->type != EXTERN) { /* unresolved */
    symbol_data->type = EXTERN;
    symbol_table->unresolved_usage_count--;
    symbol_table->extern_count++;
  }
  return SUCCESS;
}

Bool ext_ent_analyze (LineInfo *line, Symbol_Table *symbol_table)
{
  if (!run_fsm (line, symbol_table)) {
    return FALSE;
  }

  if (!IS_EMPTY(line->label)) {
    lineToPostfix (line->parts); /*get the fist tok again for the error msg */
    lineTok (line->parts);
    r_warning ("label ", line->parts, " is ignored");
  }
  return TRUE;
}

exit_code ent_analyze (LineInfo *line, Symbol_Table *symbol_table,
                       Entry_List *entry_list)
{
  char name[MAX_LINE_LEN] = "";
  line->info.ext_ent.name = name;
  line->type_l = ent_l;

  if (!ext_ent_analyze(line, symbol_table)){
    return ERROR;
  }
  return process_entry_label (line, symbol_table, entry_list);
}

exit_code ext_analyze (LineInfo *line, Symbol_Table *symbol_table)
{
  char name[MAX_LINE_LEN] = "";
  exit_code res;
  line->info.ext_ent.name = name;
  line->type_l = ext_l;

  if (!ext_ent_analyze(line, symbol_table)){
    return ERROR;
  }
  return process_extern_label (line, symbol_table);
}

/************************* operator *************************/
Opcode get_opcode (char *token)
{
  int i;
  for (i = 0; i < NUM_OF_OP; ++i) {
    if (strcmp (token, op_names[i]) == 0) {
      return i;
    }
  }
  return NO_OPCODE;
}

Symbol_N *process_operand_label (Symbol_Table *symbol_table, Operand *operand)
{
  Symbol_N *symbol = add_symbol (symbol_table, operand->info.symInx.symbol,
                                 UNRESOLVED_USAGE, 0, NOT_ENTRY);
  operand->info.symInx.symbol = symbol;
  operand->info.symInx.found = TRUE;
  symbol_table->unresolved_usage_count++;
  return symbol;
}

Bool process_code_label (Symbol_Table *symbol_table, char *label, int address)
{
  Symbol_Data *symbol_data;
  Symbol_N *symbol = find_symbol (symbol_table, label);
  if (!symbol) {
    symbol = add_symbol (symbol_table, label, CODE, address, NOT_ENTRY);
    return (symbol != NULL);
  }
  /* else, the symbol already exist.
   * since validation already done, it's must be unresolved symbol */
  symbol_data = (Symbol_Data *) symbol->data;
  update_unresolved_count (symbol_data, symbol_table);
  symbol_data->type = CODE;
  return TRUE;
}

Bool process_op_labels (Symbol_Table *symbol_table, LineInfo *line)
{
  Symbol_N *symbol;
  Operand *src = &line->info.op->src, *target = &line->info.op->target;

  /* process code symbol (label at the beginning of the line) */
  if (!IS_EMPTY(line->label)) {
    if (!process_code_label (symbol_table, line->label, line->info.op->address)) {
      return FALSE;
    }
  }
  /* src and target share the same unresolved symbol */
  if (!src->info.symInx.found && !target->info.symInx.found
      && strcmp (src->info.symInx.symbol, target->info.symInx.symbol) == 0) {
    if (!(symbol = process_operand_label (symbol_table, src))) {
      return FALSE;
    }
    target->info.symInx.symbol = symbol;
    target->info.symInx.found = TRUE;
    return TRUE;
  }
  /* src use unresolved label */
  if (!src->info.symInx.found) {
    if (!process_operand_label (symbol_table, src)) {
      return FALSE;
    }
  }
  /* target use unresolved label */
  if (!target->info.symInx.found) {
    if (!process_operand_label (symbol_table, target)) {
      return FALSE;
    }
  }
  return TRUE;
}

exit_code op_handler (LineInfo *line, Opcode opcode, file_analyze *f)
{
  char src_sym_buffer[MAX_LINE_LEN];
  char target_sym_buffer[MAX_LINE_LEN];
  op_analyze op;
  init_op_analyze (&op, opcode, src_sym_buffer, target_sym_buffer);
  line->info.op = &op;
  line->type_l = op_l;

  if (!run_fsm (line, f->symbol_table)) {
    return ERROR;
  }

  line->info.op->address = f->IC + IC_START;
  if (!process_op_labels (f->symbol_table, line)) {
    return MEMORY_ERROR;
  }

  if (!add_to_op_list (f->op_list, line)) {
    return MEMORY_ERROR;
  }

  f->IC += calc_op_size (line->info.op);
  return SUCCESS;
}

/**** else */
exit_code else_handler (LineInfo *line)
{
  char *first_word = line->parts->token;
  /* if it's valid symbol name, but with no ':' */
  if (IS_EMPTY(line->label)
      && valid_identifier (line->parts, first_word, FALSE)) {
    r_error ("expected ':' after ", line->parts, "");
  }
  else {
    r_error ("undefined: ", line->parts, "");
  }
  return ERROR;
}

/****************** process function *******************/

exit_code first_process (file_analyze *f, LineInfo *line_info)
{
  exit_code res;
  Opcode opcode;
  char *token = line_info->parts->token;

  if (strcmp (".define", token) == 0) {
    res = define_analyze (line_info, f->symbol_table);
  }
  else if (strcmp (".entry", token) == 0) {
    res = ent_analyze (line_info, f->symbol_table, f->entry_list);
  }
  else if (strcmp (".extern", token) == 0) {
    res = ext_analyze (line_info, f->symbol_table);
  }
  else if (strcmp (".string", token) == 0) {
    res = str_analyze (line_info, f);
  }
  else if (strcmp (".data", token) == 0) {
    res = data_analyze (line_info, f);
  }
  else if ((opcode = get_opcode (token)) != NO_OPCODE) {
    res = op_handler (line_info, opcode, f);
  }
  else {
    res = else_handler (line_info);
  }

/*  print_symbol_table (f->symbol_table, stdout);*/
  return res;

}

exit_code label_handler (file_analyze *f, LineInfo *line, char *label)
{
  Symbol_N *symbol;
  Symbol_Data *symbol_data;

  strcpy (line->label, line->parts->token);
  REMOVE_LAST_CHAR(line->label); /* remove ":" */
  if (!valid_identifier (line->parts, line->label, TRUE)) {
    return ERROR;
  }

  if ((symbol = find_symbol (f->symbol_table, label))) {
    symbol_data = (Symbol_Data *) symbol->data;
    if (symbol_data->type != UNRESOLVED_USAGE
        && symbol_data->type != UNRESOLVED_ENTRY
        && symbol_data->type != UNRESOLVED_ENTRY_USAGE) {
      r_error ("redeclaration of ", line->parts, "");
      return ERROR;
    }
  }
  lineTok (line->parts);
  return first_process (f, line);
}

/****************** main function *******************/

exit_code firstPass (FILE *input_file, file_analyze *f)
{
  LineInfo line_info;
  LinePart line_part;
  char prefix[MAX_LINE_LEN] = "", token[MAX_LINE_LEN] = "",
      postfix[MAX_LINE_LEN] = "", label[MAX_LINE_LEN] = "";
  exit_code res;

  init_line_parts (&line_part, prefix, token, postfix);
  res = init_first_pass (f, f->file_name, &line_info, &line_part, label);

  while (res != MEMORY_ERROR &&
         fgets (line_info.parts->line, MAX_LINE_LEN, input_file)) {
    restart_line_info (&line_info);
    lineTok (line_info.parts);

    if (IS_EMPTY(line_info.parts->token)) {
      continue;
    }
    else if (IS_LABEL(line_info.parts->token)) {
      res = label_handler (f, &line_info, label);
    }
    else {
      res = first_process (f, &line_info);
    }
    f->error += res;
  }
#ifdef DEBUG
  show_op_list (f->op_list, stdout);
  show_data_segment (f->data_segment, stdout);
  show_symbol_table (f->symbol_table, stdout);
  show_entry_list(f->entry_list, stdout);
#endif

  free_file_analyze1 (f);
  return res;
}













