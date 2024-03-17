/*
 Created by OMER on 1/15/2024.
*/

#include "firstPass.h"

#define IS_LABEL(str) (str[strlen (str) - 1] == ':')

/****************** pass helpers *******************/

/*todo change */
void free_file_analyze1 (file_analyze *f)
{
  freeList (f->symbol_table);
  free_vector (f->data_segment);
  free_op_list (f->op_list);
  free_entry_table (f->entry_table);
  memset (f, 0, sizeof (file_analyze));
}

exit_code init_first_pass (LinePart *line, char *file_name, file_analyze *f)
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
  line->file = file_name;
  line->num = 0;

  return SUCCESS;
}

/****************** handler & validation function *******************/

exit_code
str_analyze (LineInfo *line, file_analyze *file_analyze)
{
  line->type_t = str_l;
  if (!run_fsm (line, file_analyze)) {
    return ERROR;
  }

  if (IS_EMPTY(line->label)) { /* .string "a" */
    lineToPostfix (line->parts); /*get the first tok again for the error msg */
    r_warning ("", line->parts, "variables may be inaccessible without label");
  }
  else { /*add to symbol table */
    if (add_symbol (file_analyze->symbol_table, line->label, DATA,
                    file_analyze->DC, RELOCATABLE, 0) == MEMORY_ERROR) {
      return MEMORY_ERROR;
    }
  }
  /* add to data segment */
  return add_to_data_seg (file_analyze->data_segment, &file_analyze->DC,
                          CHAR_TYPE, line->str.content, line->str.len + 1);
}


/***** data */
exit_code
data_analyze (LineInfo *line, file_analyze *file_analyze)
{
  line->type_t = data_l;
  line->data.len = 0;
  if (!run_fsm (line, file_analyze)) {
    return ERROR;
  }

  if (IS_EMPTY(line->label)) { /* .data 3 */
    lineToPostfix (line->parts); /*get the fist tok again for the error msg */
    lineTok (line->parts);
    r_warning ("", line->parts, " variables may be inaccessible without label");
  }
  else { /*add to symbol table */
    if (add_symbol (file_analyze->symbol_table, line->label, DATA,
                    file_analyze->DC, RELOCATABLE, 0) == MEMORY_ERROR) {
      return MEMORY_ERROR;
    }
  }
  /* add to data segment */
  return add_to_data_seg (file_analyze->data_segment, &(file_analyze->DC),
                          INT_TYPE, line->data.arr, line->data.len);
}


/***** define */
exit_code define_analyze (LineInfo *line, file_analyze *file_analyze)
{

  /* label and define at the same line */
  if (!IS_EMPTY(line->label)) { /* LABEL: .define x=3 */
    lineToPostfix (line->parts); /*get the fist tok again for the error msg */
    lineTok (line->parts);
    r_error ("label ", line->parts, " and '.define' cannot be declared on the"
                                    " same line");
    line->error_t = TRUE;
    return ERROR;
  }

  line->type_t = def_l;
  if (!run_fsm (line, file_analyze)) {
    return ERROR;
  }

  /* todo val of define?!?! */
  return add_symbol (file_analyze->symbol_table, line->define.name, DEFINE,
                     0, RELOCATABLE, line->define.val);
}

/***** extern */
exit_code extern_analyze (LineInfo *line, file_analyze *file_analyze)
{
  Node *node = NULL;

  line->type_t = ext_l;
  if (!run_fsm (line, file_analyze)) {
    return ERROR;
  }

  node = findNode (file_analyze->symbol_table, line->ext_ent.name);
  /* if it's new extern declaration */
  if (!node) {
    if (add_symbol (file_analyze->symbol_table, line->ext_ent.name, DATA,
                    0, EXTERNAL, 0) == MEMORY_ERROR) {
      return MEMORY_ERROR;
    }
  }

  if (!IS_EMPTY(line->label)) {
    lineToPostfix (line->parts); /*get the fist tok again for the error msg */
    lineTok (line->parts);
    r_warning ("ignored label ", line->parts, " before '.extern'");
  }
  return SUCCESS;
}

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

/***** operator */
exit_code
op_handler (LineInfo *line, const char *label, Opcode opcode,
            file_analyze *f)
{
  line->type_t = op_l;
  init_op_analyze (line->op, opcode);

  if (!run_fsm (line, f)) {
    return ERROR;
  }

  line->op->address = f->IC + IC_START;
  if (!add_to_op_list (f->op_list, line->op)) {
    return MEMORY_ERROR;
  }

  if (!IS_EMPTY(label)) {
    if (add_symbol (f->symbol_table, label, OPERATION,
                    f->IC, RELOCATABLE, 0) == MEMORY_ERROR) {
      return MEMORY_ERROR;
    }
  }

  f->IC += calc_op_size (line->op);
  return SUCCESS;
}

/*** entry */
exit_code entry_analyze (LineInfo *line, file_analyze *file_analyze)
{
  line->type_t = ent_l;
  if (!run_fsm (line, file_analyze)){
    return ERROR;
  }

  /* todo: change after changing symbol table */
  if (!is_new_entry_symbol (file_analyze->entry_table, line->ext_ent.name)) {
    r_warning ("", line->parts, " has already declared in earlier line");
  }
  else { /* add to entry list */
    if (!add_to_entry_table (file_analyze->entry_table, line->ext_ent.name,
                             line->parts, 0)) {
      return MEMORY_ERROR;
    }
  }

  if (!IS_EMPTY(line->label)) {
    lineToPostfix (line->parts); /*get the fist tok again for the error msg */
    lineTok (line->parts);
    r_warning ("ignored label ", line->parts, " before '.entry'");
  }
  return SUCCESS;
}

/**** else */
exit_code else_handler (LinePart *line, const char *label)
{
  char *first_word = line->token;
  /* if it's valid symbol name, but with no ':' */
  if (IS_EMPTY(label) && valid_identifier (line, first_word, FALSE)) {
    r_error ("expected ':' after ", line, "");
  }
  else {
    r_error ("undefined: ", line, "");
  }
  return ERROR;
}

/****************** process function *******************/

exit_code first_process (file_analyze *f, LinePart *line_part, char
*label)
{
  exit_code res;
  LineInfo line_info;
  op_analyze op;
  Opcode opcode;

  line_info.parts = line_part;
  line_info.error_t = FALSE;
  if (!IS_EMPTY(label)) {
    strcpy (line_info.label, label);
  }
  else {
    NULL_TERMINATE(line_info.label, 0);
  }

  /* case: .define */
  if (strcmp (".define", line_part->token) == 0) {
    res = define_analyze (&line_info, f);
  }

    /* case: .entry */
  else if (strcmp (".entry", line_part->token) == 0) {
    res = entry_analyze (&line_info, f);
  }

    /* case: .extern */
  else if (strcmp (".extern", line_part->token) == 0) {
    res = extern_analyze (&line_info, f);
  }

    /* case: .string */
  else if (strcmp (".string", line_part->token) == 0) {
    res = str_analyze (&line_info, f);
  }

    /* case: .data */
  else if (strcmp (".data", line_part->token) == 0) {
    res = data_analyze (&line_info, f);
  }

    /* case: operator */
  else if ((opcode = get_opcode (line_part->token)) != NO_OPCODE) {
    res = op_handler (&line_info, label, opcode, f);
  }

    /* case: else */
  else {
    res = else_handler (line_part, label);
    line_info.error_t = TRUE;
  }

  if (!line_info.error_t) {
    print_line_info (&line_info, f->file_name);
  }

  return res;

}

exit_code label_handler (file_analyze *f, LinePart *line, char *label)
{
  strcpy (label, line->token);
  NULL_TERMINATE(label, strlen (label) - 1); /* remove ":" */
  if (!valid_identifier (line, label, TRUE)) {
    return ERROR;
  }
  if (findNode (f->symbol_table, label)) {
    r_error ("redeclaration of ", line, "");
    return ERROR;
  }
  lineTok (line);
  return first_process (f, line, label);
}

/****************** main function *******************/

exit_code firstPass (FILE *input_file, file_analyze *f)
{
  char label[MAX_LINE_LENGTH] = "";
  LinePart line;
  exit_code res;

  res = init_first_pass (&line, f->file_name, f);

  while (fgets (line.postfix, MAX_LINE_LENGTH, input_file)
         && res != MEMORY_ERROR) {
    restartLine (&line);
    NULL_TERMINATE(label, 0);
    trim_end (line.postfix);
    lineTok (&line);

    if (IS_EMPTY(line.token)) {
      continue;
    }
    else if (IS_LABEL(line.token)) {
      res = label_handler (f, &line, label);
    }
    else {
      res = first_process (f, &line, label);
    }
    f->error += res;
  }
  /* print_op_list (f->op_list, f->file_name); */
  print_data_segment (f->data_segment, f->DC);
  /*print_entry_table (f->entry_table, f->file_name);*/
  /*printList (f->symbol_table, stdout);*/

  return res;
}













