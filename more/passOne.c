//
// Created by OMER on 1/10/2024.
//

#include "passOne.h"
#include <stdlib.h>
#include <ctype.h>
#include "../input.h"


#define MAX_INT ( 8191) //   2^14 -1
#define MIN_INT (-8191) // -(2^14 -1)

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


LinkedList *symbols_table = NULL;
LinkedList *entry_table = NULL;
LinkedList *extern_table = NULL;
char *text_seg = NULL;
size_t DC = 0, IC = 100, line_num = 1;
LineInfo curLine;

DS_Word *data_seg; //todo check about size

//todo add file name and the whole line
void raiseError(char* cause, char* msg){
  printf("file:%-2lu ",line_num);
  printf(RED"error: " RESET"'"  YEL"%s" RESET"' ", cause);
  printf ("%s\n", msg);
  // 56 | mul a "b"
}

void raiseWarning(char* cause, char* msg){
  printf("file:%-2lu ",line_num);
  printf(CYN "warning: " RESET"'"  YEL"%s" RESET"' ", cause);
  printf ("%s\n", msg);
  // 56 | mul a "b"
}

void print_dataSeg (void)
{
  int i;
  for (i = 0; i < DC; ++i) {
    if (data_seg[i].type == INT_TYPE){
      printf ("| %d ", data_seg[i].val);
    }else{ //char
      printf ("| %c ", (char)data_seg[i].val);
    }
  }
  printf("|\n");
}

int init_pass (void)
{
  symbols_table = createList (init_symbol, print_symbol, free);
  if (!symbols_table) {
    return EXIT_FAILURE; /* memory error */
  }

  data_seg = (DS_Word *) malloc (INIT_SIZE * sizeof (DS_Word));
  if (!data_seg) {
    free (symbols_table);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int isLabel (const char *first_word)
{
  return first_word[strlen (first_word) - 1] == ':';
}

char *isValidStr (char *line)
{
  char *s, *end;
  if (line == NULL || *line == '\0') {
    return NULL;
  }

  // Skip leading and trailing whitespace
  end = line + strlen (line) - 1;
  while (*line && isspace(*line)) { line++; }
  while (end > line && isspace(*end)) { *end-- = '\0'; }

  // Must be enclosed in "" and have at least one char between them
  if (*line != '"'){
    raiseError (line, "must start with opening \"");
    return NULL;
  }
  if (*end != '"'){
    raiseError (line, "must end with closing \"");
    return NULL;
  }

  if (end - line <= 1) {
    raiseError (line, "must be enclosed in \" \" and have at least"
                                " one char between");
    return NULL;
  }

  //Make sure there are no other " in the middle
  for (s = line + 1; s < end; s++) {
    if (*s == '"') {
      raiseError (line, "extra text after closer \"\n");
      return NULL;
    }
  }
  *end = '\0';
  return line + 1; //Return a pointer to the word between the ""
}

/*int add_str_to_dataSeg (char *str, size_t size)
{
  size_t target_dc = DC + size;
  if (target_dc -1 >= 100) {
    return FALSE; //data seg is full
  }
  for (;  DC < target_dc; DC++) {
    data_seg[DC] = *str++;
  }
  return TRUE;
}*/

int addToDataSeg(DS_Type type, void *arr, size_t size)
{
  size_t target_dc = DC + size;
  int *intArr;
  char *charArr;
  if (target_dc - 1 >= 100) {
    raiseError (".data", "overflow in data segment"); //todo
    return FALSE;
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
      raiseError ("unknown data type", "");
  }
  return TRUE;
}

int add_data_symbol(char* symbol_name){
  symbol symbol_data;
  symbol_data.address = DC;
  symbol_data.type = DIRECTIVE;
  symbol_data.isExtern = FALSE;
  symbol_data.isEntry = FALSE;
  Node *new_symbol = createNode (symbols_table, symbol_name, &symbol_data);
  if (!new_symbol) {
    return FALSE; //memory error
  }
  appendToTail (symbols_table, new_symbol);
  return TRUE;
}

int str_handler (char *line, char *symbol_name)
{
  line = isValidStr (line);
  if (!line) {
    //todo add more errors
    return FALSE; //non valid str
  }
  if (symbol_name) {
    if (!add_data_symbol(symbol_name)){
      return FALSE; //wasn't able to add str to symbol table (memory error)
    }
  }
  else{
    raiseWarning(".string", "lost variable");
  }
  if (!addToDataSeg(CHAR_TYPE, line, strlen (line) + 1)) {
    return FALSE; //wasn't able to add str to data seg
  }
  return TRUE;
}

//get not null string
int isInt (char* token, int *res){
  char *end_ptr = NULL;
  long int tmp;

  tmp = strtol (token, &end_ptr, 10);
  if (*end_ptr != '\0'){
    raiseError (token, "undeclared (first use in this directive)")
    ; //todo
    return FALSE;
  }
  if (tmp > MAX_INT || tmp < MIN_INT){
    raiseError (token, "out of integer bound [-(2^14-1),2^13-1]");
    return FALSE;
  }
  *res = (int) tmp;
  return TRUE;
}

//return number of number the success find in row or 0 in error
int validData(char* line, int* arr){
  char token[MAX_TOKEN_SIZE];
  int i=0, res;

  //get first int
  line = newToken (line, token);
  if (token[0] == '\0'){ //exp: .data _
    raiseError (".data", "empty integer initializer");
    return 0;
  }
  if (!isInt (token, &res)){ //exp: .data 1,2,x
    return -1;
  }
  arr[i++] = res;
  line = newToken (line, token);

  //get comma + next int
  while (*token != '\0'){

    //get comma
    if (*token != ','){ //exp: .data 1,2 3 | .data 1 2 x
      raiseError (token, "expected ',' before token");
    }

    //get next int
    line = newToken (line, token);
    if (*token == ','){ //exp: .data 1,,2
      raiseError (token, "expected integer before ‘,’ token");
    }
    if (!isInt (token, &res)){ //exp: .data 1,2,x
      return -1;
    }
    arr[i++] = res;
    line = newToken (line, token);
  }
  return i;
}

int int_handler (char *line, char *symbol_name)
{
  int arr[MAX_LINE_SIZE];
  int res = validData(line, arr);
  if (res < 0) {
    return FALSE; //non valid numbers def
  }
  if (symbol_name) {
    if (!add_data_symbol(symbol_name)){
      return FALSE; //wasn't able to add data to symbol table (memory error)
    }
  }
  else{
    raiseWarning(".data", "lost variable");
  }
  if (!addToDataSeg(INT_TYPE, arr, res)) {
    return FALSE; //wasn't able to add str to data seg
  }
  return TRUE;
}

int
f_processLine (char *line, char *first_word, char *symbol_name)
{
  //case: .string
  if (strcmp (".string", first_word) == 0) {
    if (!str_handler (line, symbol_name)){
      return FALSE;
    }
  }

  //case: .data
  if (strcmp (".data", first_word) == 0) {
    if (!int_handler (line, symbol_name)){
      return FALSE;
    }
  }
  return TRUE;
}

int passOne (FILE *input)
{
  /*char prefix[MAX_LINE_SIZE] = "";
  char token[MAX_LINE_SIZE] = "";
  char postfix[MAX_LINE_SIZE] = "";

  curLine.prefix = prefix;
  curLine.token = token;
  curLine.postfix = (char **) &postfix;*/


  char line[MAX_LINE_SIZE], line_copy[MAX_LINE_SIZE];
  char *first_word = NULL, *curr_label = NULL, *p_line = NULL;

  if (init_pass ()) {
    return EXIT_FAILURE; //memory error;
  }

  while (fgets (line, MAX_LINE_SIZE, input)) {
    p_line = line;
    first_word = strtok (p_line, DELIM);
    curr_label = NULL;

    // if label - pass the line without the label (cut first word)
    //todo isValidLabel
    if (isLabel (first_word)) {
      curr_label = first_word;
      first_word = strtok (NULL, DELIM);
      p_line = strtok (NULL, "");
      f_processLine (p_line, first_word, curr_label);
    }
    else { //else pass the origin line (don't cut the first word)
      p_line = strtok (NULL, "");
      f_processLine (p_line, first_word, curr_label);
    }
    ++line_num;
  }
        print_dataSeg ();
  printList (symbols_table, stdout);
  freeList (symbols_table);
  return EXIT_SUCCESS;
}

/* *************************************************
* ...Function to handle the macro's linked list ...
***************************************************/

void *init_symbol (const void *data)
{
  symbol *symbol_data = (symbol *) data;
  symbol *new_symbol = (symbol *) malloc (sizeof (symbol));
  if (!new_symbol) {
    return NULL; //memory error
  }
  new_symbol->address = symbol_data->address;
  new_symbol->isExtern = symbol_data->isExtern;
  new_symbol->type = symbol_data->type;
  new_symbol->isEntry = symbol_data->isEntry;
  return new_symbol;
}

void print_symbol (const char *word, const void *data, FILE *pf)
{
  symbol *symbol_data = (symbol *) data;

  fprintf (pf, " %-15s  %-5zu ", word, symbol_data->address);

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

  fprintf (pf, " %-15s\t", (symbol_data->isExtern == TRUE ?
                                                    "external": "not external"));
  fprintf (pf, "\n");

}