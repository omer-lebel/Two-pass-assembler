//
// Created by OMER on 1/10/2024.
//

#include "passOne.h"
#include <stdlib.h>
#include <ctype.h>
#include "input.h"

/*
* first path:
* 1) symbols table:
 label | address (IC/DC) or val | type (cmd/data/mdefine) | external | entry

* 2) create data segment, compute its size DC (from 100)
* 3) create partial text segment, compute its size IC (from 0)
* 4) create external table: | label | line that appears in program
* 5.1) create entry table - text: | label | address
* 5.2) create entry table - data: | label | address
* 6) update address of data in the data segment (data is after text)
*/

LinkedList *symbols_table = NULL;
LinkedList *entry_table = NULL;
LinkedList *extern_table = NULL;
char *data_seg = NULL;
char *text_seg = NULL;
size_t DC = 0, IC = 100;

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

typedef enum symbol_type
{
    DIRECTIVE, OPERATION, DEFINE
} symbol_type;

typedef struct symbol
{
    size_t address;
    symbol_type type;
    bool isEntry;
    bool isExtern;
} symbol;


//todo add file name and the whole line
void raiseError(int line_num, char* cause, char* msg){
  printf("file:%-2d ",line_num);
  printf(RED"error " RESET"in '"  YEL"%s" RESET"' ", cause);
  printf ("%s\n", msg);
  // 56 | mul a "b"
}

void print_dataSeg ()
{
  int i;
  for (i = 0; i < DC; ++i) {
    if (data_seg[i] == '\0'){
      printf ("| \\0 ");
    }else{
      printf ("| %c ", data_seg[i]);
    }
  }
  printf("|\n");
}

int init_pass ()
{
  symbols_table = createList (init_symbol, print_symbol, free);
  if (!symbols_table) {
    return EXIT_FAILURE; /* memory error */
  }

  data_seg = (char *) malloc (INIT_SIZE * sizeof (char));
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

char *isValidStr (char *line, int line_num)
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
    raiseError (line_num, line, "must start with opening \"");
    return NULL;
  }
  if (*end != '"'){
    raiseError (line_num, line, "must end with closing \"");
    return NULL;
  }

  if (end - line <= 1) {
    raiseError (line_num, line, "must be enclosed in \" \" and have at least"
                                " one char between");
    return NULL;
  }

  //Make sure there are no other " in the middle
  for (s = line + 1; s < end; s++) {
    if (*s == '"') {
      raiseError (line_num, line, "extra text after closer \"\n");
      return NULL;
    }
  }
  *end = '\0';
  return line + 1; //Return a pointer to the word between the ""
}

int add_str_to_dataSeg (char *str, size_t size)
{
  size_t target_dc = DC + size;
  if (target_dc -1 >= 100) {
    return FALSE; //data seg is full
  }
  for (;  DC < target_dc; DC++) {
    data_seg[DC] = *str++;
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

int str_handler (char *line, int line_num, char *symbol_name)
{
  line = isValidStr (line, line_num);
  if (!line) {
    //todo add more errors
    return FALSE; //non valid str
  }
  if (symbol_name) {
    if (!add_data_symbol(symbol_name)){
      return FALSE; //wasn't able to add str to symbol table (memory error)
    }

  }
  if (!add_str_to_dataSeg (line, strlen (line) + 1)) {
    return FALSE; //wasn't able to add str to data seg
  }
  return TRUE;
}

int isValidInt (char* line, int line_num, int *arr){
  char token[MAX_TOKEN_SIZE];
  line = newToken (line, token);

}

int int_handler (char *line, int line_num, char *symbol_name)
{
  int arr[100];
  if (!isValidInt (line, line_num, arr)) {
    return FALSE; //non valid numbers
  }
  if (symbol_name) {
    if (!add_data_symbol(symbol_name)){
      return FALSE; //wasn't able to add data to symbol table (memory error)
    }

  }
  if (!add_str_to_dataSeg (line, strlen (line) + 1)) {
    return FALSE; //wasn't able to add str to data seg
  }
  return TRUE;
};

int
f_processLine (char *line, int line_num, char *first_word, char *symbol_name)
{
  //case: .string
  if (strcmp (".string", first_word) == 0) {
    if (!str_handler (line, line_num, symbol_name)){
      return FALSE;
    }
  }

  //case: .data
  if (strcmp (".data", first_word) == 0) {
    if (!int_handler (line, line_num, symbol_name)){
      return FALSE;
    }
  }
  return TRUE;
}

int passOne (FILE *input)
{
  char line[MAX_LINE_SIZE], line_copy[MAX_LINE_SIZE];
  char *first_word = NULL, *curr_label = NULL, *p_line = NULL;
  int line_num = 1;

  if (init_pass ()) {
    return FALSE; //memory error;
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
      f_processLine (p_line, line_num++, first_word, curr_label);
    }
    else { //else pass the origin line (don't cut the first word)
      p_line = strtok (NULL, "");
      f_processLine (p_line, line_num++, first_word, curr_label);
    }
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