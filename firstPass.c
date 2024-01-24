/*
 Created by OMER on 1/15/2024.
*/


#include "firstPass.h"

#define INIT_SIZE 100

LinkedList *symbols_table;
char *text_seg;
DsWord *data_seg;

size_t DC, IC;

/****************** pass helpers *******************/


int init_pass (LineInfo *line, char *prefix, char *token, char *postfix,
               char *file_name)
{
  symbols_table = createList (init_symbol, print_symbol, free);
  if (!symbols_table) {
    return FAILURE; /* memory error */
  }

  data_seg = (DsWord *) malloc (INIT_SIZE * sizeof (DsWord));
  if (!data_seg) {
    free (symbols_table);
    return FAILURE; /* memory error */
  }

  /* line info */
  line->file = file_name;
  line->num = 0;
  line->prefix = prefix;
  line->token = token;
  line->postfix = postfix;

  DC = 0;
  IC = 100;

  return SUCCESS;
}

void restartLine (char *line, LineInfo *line_info, char *label)
{
  line_info->num++;
  RESET_STR(label);
  RESET_STR(line_info->prefix);
  RESET_STR(line_info->token);
  /* postfix contain the whole line without the trim chars */
  line_info->postfix = strtok (line, "\r\n");
}

/****************** data segment *******************/

exit_code addToDataSeg (LineInfo *line, DsType type, void *arr, size_t size)
{
  size_t target_dc = DC + size;
  int *intArr;
  char *charArr;
  if (target_dc - 1 >= 100) {
    r_error ("adding the variables: ", line, " causes data segment overflow");
    return FAILURE;
  }

  switch (type) {
    case INT_TYPE:
      intArr = (int *) arr;
      for (; DC < target_dc; DC++) {
        data_seg[DC].type = type;
        data_seg[DC].val = *intArr++;
      }
      break;
    case CHAR_TYPE:
      charArr = (char *) arr;
      for (; DC < target_dc; DC++) {
        data_seg[DC].type = type;
        data_seg[DC].val = (int) *charArr++;
      }
      break;
    default:
      r_error ("unknown data type in ", line, "");
      return ERROR;
  }
  return SUCCESS;
}

void printDs (void)
{
  size_t i;
  char c;
  for (i = 0; i < DC; ++i) {
    if (data_seg[i].type == INT_TYPE) {
      printf ("| %d ", data_seg[i].val);
    }
    else if ((c = (char) data_seg[i].val) != '\0') { /* char */
      printf ("| %c ", c);
    }
    else { /* '\0' */
      printf ("|   ");
    }
  }
  if (i != 0) {
    printf ("|\n");
  }
}

/****************** symbol table *******************/

void *init_symbol (const void *data)
{
  Symbol *symbol_data = (Symbol *) data;
  Symbol *new_symbol = (Symbol *) malloc (sizeof (Symbol));
  if (!new_symbol) {
    return NULL; /* memory error */
  }
  new_symbol->address = symbol_data->address;
  new_symbol->isExtern = symbol_data->isExtern;
  new_symbol->type = symbol_data->type;
  new_symbol->isEntry = symbol_data->isEntry;
  new_symbol->val = symbol_data->val;
  return new_symbol;
}

void print_symbol (const char *word, const void *data, FILE *pf)
{
  Symbol *symbol_data = (Symbol *) data;

  fprintf (pf, " %-15s  %-5lu ", word, symbol_data->address);
  switch (symbol_data->type) {
    case DIRECTIVE:
      fprintf (pf, " %-15s", "directive");
      break;
    case OPERATION:
      fprintf (pf, " %-15s", "operation");
      break;
    case DEFINE:
      fprintf (pf, " %-15s", "define");
      break;
    default:
      fprintf (pf, " %-15s", "unknown");
  }

  fprintf (pf, " %-15s", (symbol_data->isExtern == EXTERNAL ?
                            "external" : "not external"));

  if (symbol_data->type == DEFINE){
    fprintf (pf, " %-5d", symbol_data->val);
  }
  fprintf (pf, "\n");

}

exit_code addSymbol (const char *label, SymbolType type, size_t address, int
isExtern, int val)
{
  Symbol symbol_data;
  Node *new_symbol;

  symbol_data.address = address;
  symbol_data.type = type;
  symbol_data.isExtern = isExtern;
  symbol_data.isEntry = FALSE;
  symbol_data.val = val;

  new_symbol = createNode (symbols_table, label, &symbol_data);
  if (!new_symbol) {
    return FAILURE; /* memory error */
  }
  appendToTail (symbols_table, new_symbol);
  return SUCCESS;
}

/****************** handler & validation function *******************/


/***** label */
/*todo macro*/
int isLabel (const char *str)
{
  return str[strlen (str) - 1] == ':';
}

/* todo change function name */
exit_code valid_symbol_name (LineInfo *line, char *str)
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
  if (findNode (symbols_table, str)) {
    /* todo maybe add line num */
    r_error ("", line, " has already declared in earlier line");
    return ERROR;
  }
  return SUCCESS;
}

/***** string */
void getDataTok (LineInfo *line)
{
  size_t j = 0, i = 0;
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
  if (*p == '"'){
    line->token[i] = '"';
    p++; i++;
  }

  /* copy content between */
  for (; *p && *p!='"'; i++, p++) {
    line->token[i] = *p;
  }

  /* copy ending " (if exist) */
  if (*p == '"'){
    line->token[i] = '"';
    p++; i++;
  }

  NULL_TERMINATE(line->token, i);
  line->postfix = p;
}

Bool validStr (LineInfo *line)
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
  if (!IS_EMPTY(line->postfix)){
    lineTok (line);
    r_error ("extraneous text ", line, " after terminating \" character");
    return FALSE;
  }
  return TRUE;
}

exit_code str_handler (LineInfo *line, const char *label)
{
  char str[MAX_TOKEN_SIZE];
  size_t len;
  if (!validStr (line)) {
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
  else{ /*add to symbol table */
    if (addSymbol (label, DIRECTIVE, DC, NOT_EXTERNAL, 0) == FAILURE) {
      return FAILURE; /* memory error */
    }
  }
  /* add to data segment */
  return addToDataSeg (line, CHAR_TYPE, (str + 1), len);
}


/***** data */
/*get not null string*/
int validInt (LineInfo *line, long int *res){
  char *end_ptr = NULL;
  *res = strtol (line->token, &end_ptr, 10);
  if (!IS_EMPTY(end_ptr) || *res > MAX_INT || *res < MIN_INT){
    return FALSE;
  }
  return TRUE;
}

Bool isImm(LineInfo *line, int* res){
  long int tmp = 0;
  Node *node;
  Symbol *symbol_data;

  /*check if it's a define */
  node = findNode(symbols_table, line->token);
  if (node){
    symbol_data = (Symbol*) node->data;
    if (symbol_data->type !=DEFINE){
      r_error ("label ", line, " is not a directive of define");
      return FALSE;
    }
    if (symbol_data->type ==DEFINE){
      *res = symbol_data->val;
      return TRUE;
    }
  }

  /*check if it's a integer */
  if (!validInt (line, &tmp)){
    if (tmp > MAX_INT || tmp < MIN_INT) {
      r_error ("", line, " exceeds integer bounds [-(2^14-1), 2^13-1]");
    }
    else{
      r_error ("", line, " undeclared (first use in this directive)");
    }
    return FALSE;
  }
  *res = (int)tmp;
  return TRUE;
}


exit_code data_handler(LineInfo *line, char* label){
  int tmp = 0;
  size_t i=0;
  int arr[100] = {0}; /*todo*/

  /* get first int */
  lineTok (line);
  if (IS_EMPTY(line->token)){ /* .data _ */
    strcat (line->token, " "); /*add token to msg error*/
    r_error ("", line, "empty integer initializer");
    return ERROR;
  }
  if (!isImm (line, &tmp)){ /* .data xxx */
    return ERROR;
  }
  arr[i++] = tmp;
  lineTok (line);
/*  printf("val of data: %d\n", tmp); */

  /* get comma + next int */
  while (!IS_EMPTY(line->token)){

    /* get comma */
    if (strcmp (line->token, ",") != 0){ /* exp: .data 1,2 3 | .data 1 x, 3 */
      r_error ("expected ',' before ", line, "");
      return ERROR;
    }

    /* get next int */
    lineTok (line);
    if (strcmp (line->token, ",") == 0){ /* .data 1,,2 */
      r_error ("expected integer before ", line, "token");
      return ERROR;
    }
    if (!isImm(line, &tmp)){ /* .data 1,2,xxx | .data 1, 99999999999 */
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
  else{ /*add to symbol table */
    if (addSymbol (label, DIRECTIVE, DC, NOT_EXTERNAL, 0) == FAILURE) {
      return FAILURE; /* memory error */
    }
  }
  /* add to data segment */
  return addToDataSeg (line, INT_TYPE, arr, i);
  return SUCCESS;
}




/*Bool equal_handler(LineInfo *line, const char *label);*/

/***** define */

exit_code define_handler (LineInfo *line, const char *label)
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
  if (valid_symbol_name (line, name) == ERROR) {
    /* (not saved word && not abc123 && or not new name) */
    return ERROR;
  }

  /*equal sign*/
  lineTok (line);
  if (IS_EMPTY(line->token)){
    strcat (line->token, " ");
  }
  if (strcmp (line->token, "=") != 0) { /*.define x y | .define x 3 */
    r_error ("expected '=' before numeric token, but got ", line, "");
    return ERROR;
  }

  /*number*/
  lineTok (line);
  if (IS_EMPTY(line->token)){
    strcat (line->token, " "); /*add token to msg error*/
    r_error ("expected numeric expression after '=' but got ", line, "");
    return ERROR;
  }
  if (!validInt (line, &res)){
    if (res > MAX_INT || res < MIN_INT) {
      r_error ("", line, " exceeds integer bounds [-(2^14-1), 2^13-1]");
    }
    else{
      r_error ("",line, " is not a valid numeric expression");
  }
    return ERROR;
  }

  if (!IS_EMPTY(line->postfix)){
    lineTok (line);
    r_error ("extraneous text ", line, " after directive");
    return ERROR;
  }

  /*add to table list*/
  /*todo val of define?!?! */
  if (addSymbol (name, DEFINE, 0, NOT_EXTERNAL, res) == FAILURE) {
    return FAILURE; /* memory error */
  }
  return SUCCESS;
}

/***** extern */
exit_code extern_handler (LineInfo *line, const char *label)
{
  char *symbol = line->token;
  if (IS_EMPTY (line->token)) {
    strcat (line->token, " "); /*add token to msg error*/
    r_error ("", line, " empty external declaration");
    return ERROR;
  }

  if (valid_symbol_name (line, symbol) == ERROR) {
    return ERROR;
  }
  if (!IS_EMPTY(line->postfix)) {
    lineTok (line);
    r_error ("extraneous text ", line, " after directive");
    return ERROR;
  }
  if (addSymbol (symbol, DIRECTIVE, 0, EXTERNAL, 0) == FAILURE) {
    return FAILURE; /* memory error */
  }
  if (!IS_EMPTY(label)) {
    lineToPostfix (line); /*get the fist tok again for the error msg */
    lineTok (line);
    r_warning ("ignored label", line, "before '.extern'");
  }
  return SUCCESS;
}





/****************** process function *******************/
/*todo add empty line after label*/

exit_code f_processLine (LineInfo *line, char *label)
{
  exit_code res = SUCCESS;
  /* case: .string */
  if (strcmp (".define", line->token) == 0) {
    lineTok (line);
    res = define_handler (line, label);
    /*nothing*/
  }

    /* case: .entry */
  else if (strcmp (".entry", line->token) == 0) {
    /*todo nothing ?*/
  }

    /* case: .extern */
  else if (strcmp (".extern", line->token) == 0) {
    lineTok (line);
    res = extern_handler (line, label);
    /*nothing*/
  }

    /* case: .string */
  else if (strcmp (".string", line->token) == 0) {
    getDataTok (line);
    res = str_handler (line, label);
  }

    /* case: .data */
  else if (strcmp (".data", line->token) == 0) {
    res = data_handler (line, label);
  }
  return res;
}

/****************** main function *******************/

int firstPass (FILE *input_file, char *file_name, exit_code *no_error)
{
  char line[MAX_LINE_SIZE],
      prefix[MAX_LINE_SIZE],
      token[MAX_LINE_SIZE],
      label[MAX_LINE_SIZE];
  LineInfo line_info;
  exit_code res;

  if (init_pass (&line_info, prefix, token, line, file_name) == FAILURE) {
    return EXIT_FAILURE; /* memory error; */
  }

  while (fgets (line, MAX_LINE_SIZE, input_file)) {
/*    printf("%s\n", line);*/
    restartLine (line, &line_info, label);

    lineTok (&line_info);
    if (isLabel (line_info.token)) {
      strcpy (label, line_info.token);
      NULL_TERMINATE(label, strlen (label) - 1); /* remove ":" */
      if (valid_symbol_name (&line_info, label) == ERROR) {
        *no_error += ERROR; /*todo think*/
        continue;
      }
      lineTok (&line_info);
      res = f_processLine (&line_info, label);
    }/* endIf label */
    else {
      res = f_processLine (&line_info, label);
    }

    if (res == FAILURE) {
      break;
    }
    *no_error += res;
  }

  printDs ();
  printList (symbols_table, stdout);

  /* todo free... */
  free (data_seg);
  freeList (symbols_table);

  return EXIT_SUCCESS;
}













