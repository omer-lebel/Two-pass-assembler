/*
 Created by OMER on 1/15/2024.
*/
#include "setting.h"
#include "firstPass.h"
#include "utils/text.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/memoryImg.h"
#include "fileStructures/ast.h"
#include "fileStructures/entryTable.h"

#include "fsm.h"

/****************** pass helpers *******************/

void free_file_analyze1(file_analyze *f){

  if (f->macro_list){
    freeList (f->macro_list);
  }

  if (f->symbol_table){
    freeList (f->symbol_table);
  }

  if (f->data_segment){
    free_vector (f->data_segment);
  }

  if (f->op_list){
    free_op_list(f->op_list);
  }

  if (f->entry_table){
    free_entry_table (f->entry_table);
  }

  memset(f, 0, sizeof(file_analyze));
}


exit_code init_first_pass (LineInfo *line, char *file_name, file_analyze *f)
{
  f->symbol_table = init_symbol_table ();
  f->data_segment = init_data_seg (&f->DC);
  f->op_list = init_op_list ();
  f->entry_table = init_entry_table ();
  if (!f->symbol_table || !f->data_segment || !f->op_list || !f->entry_table) {
    free_file_analyze1(f);
    return MEMORY_ERROR;
  }

  /* line info */
  line->file = file_name;
  line->num = 0;

  return SUCCESS;
}

void restartLine (LineInfo *line_info, char *label)
{
  line_info->num++;
  RESET_STR(label);
  RESET_STR(line_info->prefix);
  RESET_STR(line_info->token);
  /* postfix contain the whole line without the trim chars */
  trim_end (line_info->postfix);
}

/****************** handler & validation function *******************/


/***** label */


/* todo change function name */
exit_code valid_symbol_name (LineInfo *line, char *str, LinkedList
*symbol_table)
{
  if (!isalpha(str[0])) {
    r_error ("", line, " starts with a non-alphabetic character");
    return ERROR;
  }
  if (!isAlphaNumeric (str)) {
    r_error ("", line, " contains non-alphanumeric characters");
    return ERROR;
  }
  /* todo check */
  if (isSavedWord (str)) {
    r_error ("", line, " is a reserved keyword that cannot be used as an "
                       "identifier");
    return ERROR;
  }
  return SUCCESS;
}

/***** string */
void get_data_tok (LineInfo *line)
{
  size_t j, i = 0;
  char *p = (line->postfix);

  /* Concatenate prefix and token */
  strcat (line->prefix, line->token);
  j = strlen (line->prefix);

  /* skip empty characters in postfix and write them in prefix */
  for (; isspace(*p); j++, p++) {
    line->prefix[j] = *p;
  }
  NULL_TERMINATE(line->prefix, j);

  /* copy opening " (if exist) */
  if (*p == '"') {
    line->token[i] = '"';
    p++;
    i++;
  }

  /* copy content between */
  for (; *p && *p != '"'; i++, p++) {
    line->token[i] = *p;
  }

  /* copy ending " (if exist) */
  if (*p == '"') {
    line->token[i] = '"';
    p++;
    i++;
  }

  NULL_TERMINATE(line->token, i);
  strcpy (line->postfix, p);
}

Bool is_str (LineInfo *line)
{
  char *start = line->token;
  size_t len = strlen (start);
  char *end = start + len - 1;

  if (IS_EMPTY (line->token)) { /* .string _ */
    strcat (line->token, " "); /*add token to msg error*/
    r_error ("", line, " empty string initialization");
    return FALSE;
  }
  if (*start != '"') { /* .string a" */
    r_error ("missing opening \" character in ", line, "");
    return FALSE;
  }
  if (*end != '"') { /*.string "a */
    r_error ("missing terminating \" character in ", line, "");
    return FALSE;
  }
  if (!IS_EMPTY(line->postfix)) {
    lineTok (line);
    r_error ("extraneous text ", line, " after terminating \" character");
    return FALSE;
  }
  return TRUE;
}

exit_code
str_handler (LineInfo *line, const char *label, file_analyze *file_analyze)
{
  char str[MAX_TOKEN_SIZE];
  size_t len;
  if (!is_str (line)) {
    return ERROR;
  }
  strcpy (str, line->token);
  len = strlen (str);
  NULL_TERMINATE(str, --len); /* remove '"' char */

  if (!IS_EMPTY(line->postfix)) { /* L: .string "a" xxx */
    lineTok (line);
    r_error ("extraneous text ", line, " after directive");
    return ERROR;
  }

  if (IS_EMPTY(label)) { /* .string "a" */
    lineToPostfix (line); /*get the fist tok again for the error msg */
    lineTok (line);
    r_warning ("", line, " variables may be inaccessible without label");
  }
  else { /*add to symbol table */
    if (add_symbol (file_analyze->symbol_table, label, DATA,
                    file_analyze->DC, RELOCATABLE, 0) == MEMORY_ERROR) {
      return MEMORY_ERROR;
    }
  }
  /* add to data segment */
  return add_to_data_seg (file_analyze->data_segment, &file_analyze->DC, line,
                          CHAR_TYPE, (str + 1), len);
}


/***** data */
/*get not null string*/
int is_int (LineInfo *line, long int *res)
{
  char *end_ptr = NULL;
  *res = strtol (line->token, &end_ptr, 10);
  if (!IS_EMPTY(end_ptr) || *res > MAX_INT || *res < MIN_INT) {
    return FALSE;
  }
  return TRUE;
}

Bool is_imm (LineInfo *line, int *res, LinkedList *symbol_table)
{
  long int tmp = 0;
  Node *node;
  Symbol *symbol_data;

  /*check if it's a define */
  node = findNode (symbol_table, line->token);
  if (node) {
    symbol_data = (Symbol *) node->data;
    if (symbol_data->type != DEFINE) {
      r_error ("label ", line, " is not a directive of define");
      return FALSE;
    }
    if (symbol_data->type == DEFINE) {
      *res = symbol_data->val;
      return TRUE;
    }
  }

  /*check if it's a integer */
  if (!is_int (line, &tmp)) {
    if (tmp > MAX_INT || tmp < MIN_INT) {
      r_error ("", line, " exceeds integer bounds [-(2^14-1), 2^13-1]");
    }
    else {
      r_error ("", line, " undeclared (first use in this directive)");
    }
    return FALSE;
  }
  *res = (int) tmp;
  return TRUE;
}

exit_code
data_handler (LineInfo *line, char *label, file_analyze *file_analyze)
{
  int tmp = 0;
  size_t i = 0;
  int arr[100] = {0}; /*todo*/

  /* get first int */
  lineTok (line);
  if (IS_EMPTY(line->token)) { /* .data _ */
    strcat (line->token, " "); /*add token to msg error*/
    r_error ("", line, "empty integer initializer");
    return ERROR;
  }
  if (!is_imm (line, &tmp, file_analyze->symbol_table)) { /* .data xxx */
    return ERROR;
  }
  arr[i++] = tmp;
  lineTok (line);
/*  printf("val of data: %d\n", tmp); */

  /* get comma + next int */
  while (!IS_EMPTY(line->token)) {

    /* get comma */
    if (strcmp (line->token, ",") != 0) { /* exp: .data 1,2 3 | .data 1 x, 3 */
      r_error ("expected ',' before ", line, "");
      return ERROR;
    }

    /* get next int */
    lineTok (line);
    if (strcmp (line->token, ",") == 0) { /* .data 1,,2 */
      r_error ("expected integer before ", line, "token");
      return ERROR;
    }
    if (!is_imm (line, &tmp, file_analyze->symbol_table)) { /* .data 1,2,xxx | .data 1, 99999999999 */
      return ERROR;
    }
    arr[i++] = tmp;
    lineTok (line);
  } /* end of while */

  if (IS_EMPTY(label)) { /* .data 3 */
    lineToPostfix (line); /*get the fist tok again for the error msg */
    lineTok (line);
    r_warning ("", line, " variables may be inaccessible without label");
  }
  else { /*add to symbol table */
    if (add_symbol (file_analyze->symbol_table, label, DATA,
                    file_analyze->DC, RELOCATABLE, 0) == MEMORY_ERROR) {
      return MEMORY_ERROR;
    }
  }
  /* add to data segment */
  return add_to_data_seg (file_analyze->data_segment, &(file_analyze->DC), line,
                          INT_TYPE, arr, i);
}




/*Bool equal_handler(LineInfo *line, const char *label);*/

/***** define */

exit_code define_handler (LineInfo *line, const char *label, LinkedList
*symbol_table)
{
  long int res = 0;
  char name[MAX_LINE_SIZE];
  strcpy (name, line->token);

  /* label and define at the same line */
  if (!IS_EMPTY(label)) { /* LABEL: .define x=3 */
    lineToPostfix (line); /*get the fist tok again for the error msg */
    lineTok (line);
    r_error ("label ", line, " and '.define' cannot be declared on the same "
                             "line");
    return ERROR;
  }

  /*define symbol name*/
  if (IS_EMPTY(name)) { /* .define _ */
    strcat (line->token, " "); /*add token to msg error*/
    r_error ("no value given in define directive ", line, "");
    return ERROR;
  }
  if (valid_symbol_name (line, name, symbol_table) == ERROR) {
    /* (not saved word && not abc123 && or not new name) */
    return ERROR;
  }
  if (findNode (symbol_table, name)) {
    r_error ("redeclaration of ", line, "");
    return ERROR;
  }

  /*equal sign*/
  lineTok (line);
  if (IS_EMPTY(line->token)) {
    strcat (line->token, " ");
  }
  if (strcmp (line->token, "=") != 0) { /*.define x y | .define x 3 */
    r_error ("expected '=' before numeric token, but got ", line, "");
    return ERROR;
  }

  /*number*/
  lineTok (line);
  if (IS_EMPTY(line->token)) {
    strcat (line->token, " "); /*add token to msg error*/
    r_error ("expected numeric expression after '=' but got ", line, "");
    return ERROR;
  }
  if (!is_int (line, &res)) {
    if (res > MAX_INT || res < MIN_INT) {
      r_error ("", line, " exceeds integer bounds [-(2^14-1), 2^13-1]");
    }
    else {
      r_error ("", line, " is not a valid numeric expression");
    }
    return ERROR;
  }

  if (!IS_EMPTY(line->postfix)) {
    lineTok (line);
    r_error ("extraneous text ", line, " after directive");
    return ERROR;
  }

  /*add to table list*/
  /*todo val of define?!?! */
  if (add_symbol (symbol_table, name, DEFINE, 0, RELOCATABLE, (int) res) ==
      MEMORY_ERROR) {
    return MEMORY_ERROR;
  }
  return SUCCESS;
}

/***** extern */
exit_code extern_handler (LineInfo *line, const char *label, LinkedList
*symbol_table)
{
  Node *node = NULL;
  Symbol *symbol;
  char *ext_label = line->token;
  if (IS_EMPTY (line->token)) {
    strcat (line->token, " "); /*add token to msg error*/
    r_error ("", line, " empty external declaration");
    return ERROR;
  }

  if (valid_symbol_name (line, ext_label, symbol_table) == ERROR) {
    return ERROR;
  }
  node = findNode (symbol_table, ext_label);
  if (node) {
    symbol = (Symbol *) node->data;
    if (symbol->are != EXTERNAL) {
      r_error ("redeclaration of ", line, "");
      return ERROR;
    }
    else {
      r_warning ("", line, " has already declared in earlier line");
    }
  }

  if (!IS_EMPTY(line->postfix)) {
    lineTok (line);
    r_error ("extraneous text ", line, " after directive");
    return ERROR;
  }
  //if it's new extern declaration
  if (!node) {
    if (add_symbol (symbol_table, ext_label, DATA, 0, EXTERNAL, 0) ==
        MEMORY_ERROR) {
      return MEMORY_ERROR;
    }
  }

  if (!IS_EMPTY(label)) {
    lineToPostfix (line); /*get the fist tok again for the error msg */
    lineTok (line);
    r_warning ("ignored label ", line, " before '.extern'");
  }
  return SUCCESS;
}

//todo get op_propriety ad arg???
Opcode get_opcode(char* token){
  int i;
  for (i=0; i<NUM_OF_OP; ++i){
    if (strcmp (token, op_propriety[i].name) == 0){
      return op_propriety[i].opcode;
    }
  }
  return NO_OPCODE;
}

/***** mov */
exit_code
op_handler (LineInfo *line, const char *label, Opcode opcode, file_analyze *f)
{
  op_analyze op;
  init_op_analyze (&op, opcode, line);

  if (!run_fsm (&op, f)) {
    return ERROR;
  }

  op.address = f->IC + INIT_IC;
  if (!add_to_op_list (f->op_list, &op)) {
    return MEMORY_ERROR;
  }

  if (!IS_EMPTY(label)) {
    if (add_symbol (f->symbol_table, label, OPERATION,
                    f->IC, RELOCATABLE, 0) == MEMORY_ERROR) {
      return MEMORY_ERROR;
    }
  }
  /*print_op_analyze(&op);*/

  f->IC += calc_op_size (&op);
  return SUCCESS;
}

/*** entry */
exit_code entry_handler (LineInfo *line, const char *label,
                         vector *entry_table, LinkedList *symbol_table)
{
  char *ent_name = line->token;
  if (IS_EMPTY (line->token)) {
    strcat (line->token, " "); /*add token to msg error*/
    r_error ("", line, " empty entry declaration");
    return ERROR;
  }

  if (valid_symbol_name (line, ent_name, symbol_table) == ERROR) {
    return ERROR;
  }
  if (!IS_EMPTY(line->postfix)) {
    lineTok (line);
    r_error ("extraneous text ", line, " after directive");
    return ERROR;
  }
  if (!is_new_entry_symbol (entry_table, ent_name)){
    r_warning ("", line, " has already declared in earlier line");
  }
  else{ //add to entry list
    if (!add_to_entry_table (entry_table, ent_name, line, 0)) {
      return MEMORY_ERROR;
    }
  }

  if (!IS_EMPTY(label)) {
    lineToPostfix (line); /*get the fist tok again for the error msg */
    lineTok (line);
    r_warning ("ignored label ", line, " before '.entry'");
  }
  return SUCCESS;
}

/**** else */
exit_code else_handler (LineInfo *line, const char *label)
{
  char *first_word = line->token;
  //if it's valid symbol name, but with no ':'
  if (IS_EMPTY(label) && isalpha(first_word[0]) && isAlphaNumeric
      (first_word) && !isSavedWord (first_word)) {
    r_error ("expected ':' after ", line, "");
  }
  else {
    r_error ("undefined: ", line, "");
  }
  return ERROR;
}


/****************** process function *******************/
/*todo add empty line after label*/

exit_code first_process (file_analyze *f, LineInfo *line, char
*label)
{
  exit_code res;
  Opcode opcode;

  /* case: .string */
  if (strcmp (".define", line->token) == 0) {
    lineTok (line);
    res = define_handler (line, label, f->symbol_table);
    /*nothing*/
  }

    /* case: .entry */
  else if (strcmp (".entry", line->token) == 0) {
    lineTok (line);
    res = entry_handler (line, label, f->entry_table, f->symbol_table);
  }

    /* case: .extern */
  else if (strcmp (".extern", line->token) == 0) {
    lineTok (line);
    res = extern_handler (line, label, f->symbol_table);
  }

    /* case: .string */
  else if (strcmp (".string", line->token) == 0) {
    get_data_tok (line);
    res = str_handler (line, label, f);
  }

    /* case: .data */
  else if (strcmp (".data", line->token) == 0) {
    res = data_handler (line, label, f);
  }

    /* case: operator */
  else if ((opcode = get_opcode (line->token)) != NO_OPCODE) {
    res = op_handler (line, label, opcode, f);
  }

    /* case: else */
  else {
    res = else_handler (line, label);
  }
  return res;

}

exit_code label_handler (LineInfo *line, char *label, LinkedList *symbol_table)
{
  strcpy (label, line->token);
  NULL_TERMINATE(label, strlen (label) - 1); /* remove ":" */
  if (valid_symbol_name (line, label, symbol_table) == ERROR) {
    return ERROR;
  }
  if (findNode (symbol_table, label)) {
    r_error ("redeclaration of ", line, "");
    return ERROR;
  }
  lineTok (line);
  return SUCCESS;
}

/****************** main function *******************/

int firstPass (FILE *input_file, file_analyze *f)
{
  char label[MAX_LINE_SIZE] = "";
  LineInfo line;
  exit_code res;

  if (init_first_pass (&line, f->file_name, f) == MEMORY_ERROR) {
    return EXIT_FAILURE; /* memory error; */
  }

  while (fgets (line.postfix, MAX_LINE_SIZE, input_file)) {
    restartLine (&line, label);
    lineTok (&line);

    if (isLabel (line.token)) {
      res = label_handler (&line, label, f->symbol_table);
      if (res != SUCCESS) {
        f->error += res;
        continue;
      }
      res = first_process (f, &line, label);
    }/* endIf label */
    else {
      res = first_process (f, &line, label);
    }

    if (res == MEMORY_ERROR) {
      break;
    }
    f->error += res;
  }
  print_op_list (f->op_list, f->file_name);
  print_data_segment (f->data_segment, f->DC);
  /*print_entry_table (f->entry_table, f->file_name);*/
  /*printList (file_analyze->symbol_table, stdout);*/

  return EXIT_SUCCESS;
}













