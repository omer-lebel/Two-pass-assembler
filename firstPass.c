/*
 Created by OMER on 1/15/2024.
*/

#include "firstPass.h"

#define IS_LABEL(str) (str[strlen (str) - 1] == ':')



/************************* pass helpers *************************/

/*todo change */
void free_file_analyze1 (file_analyze *f)
{
  freeList (f->symbol_table);
  free_vector (f->data_segment);
  free_op_list (f->op_list);
  free_entry_table (f->entry_table);
  memset (f, 0, sizeof (file_analyze));
}

exit_code init_first_pass (file_analyze *f, char *file_name,
                           LineInfo *line_info, LinePart *line_part,
                           char *label)
{
  f->symbol_table = init_symbol_table ();
  f->data_segment = init_data_seg (&f->DC);
  f->op_list = init_op_list ();
  f->entry_table = init_entry_table ();
  if (!f->symbol_table || !f->data_segment || !f->op_list || !f->entry_table) {
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

/************************* data or string *************************/
exit_code process_directive_label (LineInfo *line, LinkedList *symbol_table,
                                   int DC)
{
  Symbol *symbol;
  Node *node = findNode (symbol_table, line->label);
  if (!node) {
    if (!add_symbol (symbol_table, line->label, DATA, DC, FALSE)) {
      return MEMORY_ERROR;
    }
    return SUCCESS;
  }

  /* else label already exist */
  symbol = (Symbol *) node->data;
  if (symbol->type != UNRESOLVED) {
    r_error ("redeclaration of ", line->parts, "");
    return ERROR;
  }
  else {
    symbol->type = DATA;
    symbol->val = DC;
  }
  return SUCCESS;
}

Bool directive_analyze (LineInfo *line, file_analyze *file_analyze)
{
  if (!run_fsm (line, file_analyze)) {
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

  if (!directive_analyze (line, f)) {
    return ERROR;
  }
  /* add or update symbol table */
  res = process_directive_label (line, f->symbol_table, f->DC);
  if (res != SUCCESS) {
    return res;
  }
  /* add to data seg */
  if (!add_to_data_seg (f->data_segment, &(f->DC),CHAR_TYPE,
                        line->info.str.content, line->info.str.len + 1)) {
    return MEMORY_ERROR;
  }

#ifdef DEBUG
  print_line_info (line, f->file_name);
#endif
  return SUCCESS;
}

/************************* data *************************/
exit_code data_analyze (LineInfo *line, file_analyze *f)
{
  int data_arr[MAX_DATA_ARR_LENGTH];
  exit_code res;
  line->type_l = data_l;
  line->info.data.arr = data_arr;

  if (!directive_analyze (line, f)) {
    return ERROR;
  }
  /* add or update symbol table */
  res = process_directive_label (line, f->symbol_table, f->DC);
  if (res != SUCCESS) {
    return res;
  }

  /* add to data seg */
  if (!add_to_data_seg (f->data_segment, &(f->DC),
                        INT_TYPE, line->info.data.arr, line->info.data.len)) {
    return MEMORY_ERROR;
  }

#ifdef DEBUG
  print_line_info (line, f->file_name);
#endif
  return SUCCESS;
}

/************************* define *************************/
exit_code define_analyze (LineInfo *line, file_analyze *file_analyze)
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
  if (!run_fsm (line, file_analyze)) {
    return ERROR;
  }

  if ((findNode (file_analyze->symbol_table, line->info.define.name))) {
    get_identifier_tok (line->parts, !IS_EMPTY(line->label));
    r_error ("redeclaration of ", line->parts, "");
    return ERROR;
  }
  if (!add_symbol (file_analyze->symbol_table, line->info.define.name,
                   DEFINE, line->info.define.val, FALSE)) {
    return MEMORY_ERROR;
  }
#ifdef DEBUG
  print_line_info (line, file_analyze->file_name);
#endif
  return SUCCESS;
}

/************************* ent ext *************************/
exit_code ext_ent_analyze (LineInfo *line, file_analyze *file_analyze)
{
  if (!run_fsm (line, file_analyze)) {
    return ERROR;
  }

  if (!IS_EMPTY(line->label)) {
    lineToPostfix (line->parts); /*get the fist tok again for the error msg */
    lineTok (line->parts);
    r_warning ("label ", line->parts, " is ignored");
  }
  return SUCCESS;
}

/************************* entry *************************/
exit_code process_entry_label (LinkedList *symbol_table, LineInfo *line)
{
  Symbol *symbol;
  Node *node = findNode (symbol_table, line->info.ext_ent.name);
  if (!node) {
    if (!(add_symbol (symbol_table, line->info.ext_ent.name, UNRESOLVED, 0, TRUE))) {
      return MEMORY_ERROR;
    }
    return SUCCESS;
  }

  /* else, label already exist */
  symbol = (Symbol *) node->data;
  get_identifier_tok (line->parts, !IS_EMPTY(line->label));

  if (symbol->type == DEFINE || symbol->type == EXTERN) {
    r_error ("redeclaration of ", line->parts, "");
    return ERROR;
  }

  /* label that can be entry label: */
  if (symbol->isEntry) {
    r_warning ("", line->parts, " has already declared in earlier line");
  } /*else */
  symbol->isEntry = TRUE;
  return SUCCESS;
}

exit_code entry_analyze (LineInfo *line, file_analyze *file_analyze)
{
  char ent_name[MAX_LINE_LEN] = "";
  exit_code res;
  line->info.ext_ent.name = ent_name;
  line->type_l = ent_l;

  if (ext_ent_analyze (line, file_analyze) != SUCCESS) {
    return ERROR;
  }

  res = process_entry_label (file_analyze->symbol_table, line);
  if (res != SUCCESS) {
    return res;
  }

#ifdef DEBUG
  print_line_info (line, file_analyze->file_name);
#endif
  return SUCCESS;
}

/************************* extern *************************/
exit_code process_extern_label (LinkedList *symbol_table, LineInfo *line)
{
  Symbol *symbol;
  Node *node = findNode (symbol_table, line->info.ext_ent.name);
  if (!node) { /*new symbol */
    if (!(add_symbol (symbol_table, line->info.ext_ent.name, EXTERN, 0, FALSE))) {
      return MEMORY_ERROR;
    }
    return SUCCESS;
  }

  /* else, label already exist */
  symbol = (Symbol *) node->data;
  if (symbol->type == UNRESOLVED && !symbol->isEntry) {
    symbol->type = EXTERN;
    return SUCCESS;
  }
  /* label is resolved/entry, therefore there is an error or warning: */
  get_identifier_tok (line->parts, !IS_EMPTY(line->label));
  if (symbol->type == EXTERN) {
    r_warning ("", line->parts, " has already declared in earlier line");
    return SUCCESS;
  }
  r_error ("redeclaration of ", line->parts, "");
  return ERROR;
}

exit_code extern_analyze (LineInfo *line, file_analyze *file_analyze)
{
  char ext_name[MAX_LINE_LEN] = "";
  exit_code res;
  line->info.ext_ent.name = ext_name;
  line->type_l = ext_l;

  if (ext_ent_analyze (line, file_analyze) != SUCCESS) {
    return ERROR;
  }

  res = process_extern_label (file_analyze->symbol_table, line);
  if (res != SUCCESS) {
    return ERROR;
  }

#ifdef DEBUG
  print_line_info (line, file_analyze->file_name);
#endif
  return SUCCESS;
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

Node *process_operand_label (LinkedList *symbol_table, Operand *operand)
{
  Node *node = add_symbol (symbol_table, operand->info.symInx.symbol,
                           UNRESOLVED, 0,FALSE);
  operand->info.symInx.symbol = node;
  operand->info.symInx.found = TRUE;
  return node;
}

Bool process_code_label (LinkedList *symbol_table, char *label, int address)
{
  Symbol *symbol;
  Node *node = findNode (symbol_table, label);
  if (!node) {
    node = add_symbol (symbol_table, label, CODE, address, FALSE);
    return (node != NULL);
  }
  /* else, the symbol already exist.
   * since validation already done, it's must be unresolved symbol */
  symbol = (Symbol *) node->data;
  symbol->type = CODE;
  return TRUE;
}

Bool process_op_labels (LinkedList *symbol_table, LineInfo *line)
{
  Node *node;
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
    if (!(node = process_operand_label (symbol_table, src))) {
      return FALSE;
    }
    target->info.symInx.symbol = node;
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

  if (!run_fsm (line, f)) {
    return ERROR;
  }

  line->info.op->address = f->IC + IC_START;
  if (!process_op_labels (f->symbol_table, line)) {
    return MEMORY_ERROR;
  }

/*  if (!add_to_op_list (f->op_list, line->op)) {
    return MEMORY_ERROR;
  }*/


  f->IC += calc_op_size (line->info.op);
#ifdef DEBUG
  print_line_info (line, f->file_name);
#endif
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
    res = define_analyze (line_info, f);
  }
  else if (strcmp (".entry", token) == 0) {
    res = entry_analyze (line_info, f);
  }
  else if (strcmp (".extern", token) == 0) {
    res = extern_analyze (line_info, f);
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

  return res;

}

exit_code label_handler (file_analyze *f, LineInfo *line, char *label)
{
  Node *node;
  Symbol *symbol;

  strcpy (line->label, line->parts->token);
  REMOVE_LAST_CHAR(line->label); /* remove ":" */
  if (!valid_identifier (line->parts, line->label, TRUE)) {
    return ERROR;
  }

  if ((node = findNode (f->symbol_table, label))) {
    symbol = (Symbol *) node->data;
    if (symbol->type != UNRESOLVED) {
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

  while (fgets (line_info.parts->line, MAX_LINE_LEN, input_file)
         && res != MEMORY_ERROR) {
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
  /* print_op_list (f->op_list, f->file_name); */
  print_data_segment (f->data_segment, f->DC);
  /*print_entry_table (f->entry_table, f->file_name);*/
  printf ("\n----------------- symbol table -----------------\n");
  printList (f->symbol_table, stdout);
#endif
  free_file_analyze1 (f);

  return res;
}













