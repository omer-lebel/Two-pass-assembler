//
// Created by OMER on 1/15/2024.
//

#include "firstPass.h"

#define INIT_SIZE 100

LinkedList *symbols_table;
char *text_seg;
DsWord *data_seg;

size_t DC = 0,
      IC = 100;


/****************** pass helpers *******************/


int init_pass (LineInfo *line, char* prefix, char* token, char* postfix,
               char*file_name)
{
  symbols_table = createList (init_symbol, print_symbol, free);
  if (!symbols_table) {
    return FATAL; /* memory error */
  }

  data_seg = (DsWord *) malloc (INIT_SIZE * sizeof (DsWord));
  if (!data_seg) {
    free (symbols_table);
    return FATAL; /* memory error */
  }

  //line info
  line->file = file_name;
  line->num = 1;
  line->prefix = prefix;
  line->token = token;
  line->postfix = postfix;

  return TRUE;
}

void restartLine(LineInfo *line){
  line->num++;
  line->prefix[0] = '\0';
  line->token[0] = '\0';
  line->postfix[0] = '\0';
}

/****************** data segment *******************/

int addToDataSeg(LineInfo *line, DsType type, void *arr, size_t size)
{
  size_t target_dc = DC + size;
  int *intArr;
  char *charArr;
  if (target_dc - 1 >= 100) {
    r_error("adding the variables:", line, "causes data segment overflow");
    return FATAL;
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
      r_error("unknown data type in", line, "");
      return FALSE;
  }
  return TRUE;
}

void printDs (void)
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

/****************** symbol table *******************/

void *init_symbol (const void *data)
{
  Symbol *symbol_data = (Symbol *) data;
  Symbol *new_symbol = (Symbol *) malloc (sizeof (Symbol));
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
  Symbol *symbol_data = (Symbol *) data;

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


bool addSymbol(const char* label, SymbolType type, size_t address){
  Symbol symbol_data;
  symbol_data.address = address;
  symbol_data.type = type;
  symbol_data.isExtern = FALSE;
  symbol_data.isEntry = FALSE;
  Node *new_symbol = createNode (symbols_table, label, &symbol_data);
  if (!new_symbol) {
    return FATAL; //memory error
  }
  appendToTail (symbols_table, new_symbol);
  return TRUE;
}

/****************** handler & validation function *******************/


/***** label */
int isLabel (const char *str)
{
  return str[strlen (str) - 1] == ':';
}

bool validLabel(LineInfo *line, char *str){
  //todo check
/*  if (isSavedWord (str)){
    r_error (line, "is a saved word");
    return FALSE;
  }*/
  if (findNode (symbols_table, str)){
    r_error ("label", line, "was already declared");
    return FALSE;
  }
  return TRUE;
}

/***** string */

void getDataTok(LineInfo *line){
  size_t j, i = 0;
  char *p = (line->postfix);

  /* Concatenate prefix and token */
  strcat(line->prefix, line->token);
  j = strlen (line->prefix);

  /* skip empty characters in postfix and write them in prefix */
  for (; isspace(*p); j++, p++){
    line->prefix[j] = *p;
  }
  line->prefix[j] = '\0';

  strcpy (line->token, line->postfix);

  /* empty the  postfix */
  line->postfix[0] = '\0';
}

bool validStr(LineInfo *line){
  char *start = line->token;
  size_t len = strlen (start);
  char *end = start + len - 1;
  if (*start == '\0'){ //.string _
    r_error ("", line, "empty string initialization");
    return FALSE;
  }
  if (*start != '"'){ //.string hello"
    r_error ("missing opening \" character in", line, "");
    return FALSE;
  }
  if (strchr(start+1,'"')!=end){ //.string "hello"world"
    r_error ("redundant text after terminating \" character in", line, "");
    return FALSE;
  }
  if (*end != '"'){ //.string "hello
    r_error ("missing terminating \" character in", line, "");
    return FALSE;
  }
  return TRUE;
}

bool str_handler(LineInfo *line, const char* label){
  char str[MAX_TOKEN_SIZE];
  size_t len;
  if (!validStr (line)){
    return FALSE;
  }
  strcpy (str, line->token);
  len = strlen (str);
  str[--len] = '\0'; //remove '"' char

  //add to data symbol or raise warning
  if (label){
    if (addSymbol (label, DIRECTIVE, DC) == FATAL){
      return FATAL; //memory error
    }
  }
  else{
    r_warning ("variables define in ", line, "are lost, won't be able to "
                                             "access later");
  }

  //add to data segment
  return addToDataSeg (line, CHAR_TYPE, (str+1), len);
}


/***** data */
bool data_handler(LineInfo *line, char* label){

  return TRUE;
}

/***** define */
bool define_handler(LineInfo *line, char* label){

  return TRUE;
}


/****************** process function *******************/

bool
f_processLine (LineInfo *line, char *label)
{
  bool res;
  //case: .string
  if (strcmp (".string", line->token) == 0) {
//    lineTok (line);
    res = str_handler (line, label);
    if (res != TRUE){
      return res;
    }
  }

  //case: .data
  else if (strcmp (".data", line->token) == 0) {
//    if (!int_handler (line, label)){
//      return FALSE;
//    }
  }
  return TRUE;
}




/****************** main function *******************/

int firstPass(FILE *input_file, char* file_name){
  char prefix[MAX_LINE_SIZE] = "",
       token[MAX_LINE_SIZE] = "",
       postfix[MAX_LINE_SIZE] = "",
       label[MAX_LINE_SIZE] = "";
  LineInfo line_info;
  bool errors = TRUE;
  bool res;

  if (init_pass (&line_info, prefix, token, postfix, file_name) == FATAL) {
    return EXIT_FAILURE; //memory error;
  }

  while (fgets (line_info.postfix, MAX_LINE_SIZE, input_file)) {
    //postfix contain the whole line without the trim chars
    line_info.postfix = strtok(line_info.postfix, "\r\nEOF");

    lineTok (&line_info);
    if (isLabel (line_info.token)){
      if (validLabel (&line_info, line_info.token) == FALSE){
        continue;
      }
      strcpy (label, line_info.token);
      label[strlen (label) - 1] = '\0'; //remove ":"
      lineTok (&line_info);
      res = f_processLine (&line_info, label);
    }//endIf label
    else{
      lineTok (&line_info);
      res = f_processLine (&line_info, label);
    }

    if (res == FATAL){
      break;
    }
    errors += res;
    restartLine(&line_info);
  }

  printDs ();
  printList (symbols_table, stdout);
//  free(data_seg);
  //todo free...


  if (errors > 0){
    return EXIT_FAILURE;
  }
    return EXIT_SUCCESS;

}













