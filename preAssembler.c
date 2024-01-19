#include "preAssembler.h"

#define IS_COMMENT(s) ((s)[0] == ';')
#define IS_MCR_DEF(s) (strcmp((s), "mcr") == 0)
#define IS_MCR_END(s) (strcmp((s), "endmcr") == 0)



int preAssembler (FILE *input, FILE *output)
{
  char line[MAX_LINE_SIZE], line_copy[MAX_LINE_SIZE];
  char *first_word = NULL;
  Node *curr_mcr = NULL;
  int line_num = 1;
  LinkedList *mcr_list = createList (init_mcrData, print_mcrData, free_mcrData);
  if (!mcr_list) {
    return EXIT_FAILURE; /* memory error */
  }

  while (fgets (line, MAX_LINE_SIZE, input)) {
    strcpy (line_copy, line);
    first_word = strtok (line_copy, DELIM);

    if (!p_processLine (output, mcr_list, line, line_num, first_word,
                        &curr_mcr)) {
      freeList (mcr_list);
      return EXIT_FAILURE;
    }
  }
/*  printList (mcr_list, stdout); */
  freeList (mcr_list);
  return EXIT_SUCCESS;
}


int p_processLine (FILE *output, LinkedList *mcr_list, char *line,
                   int line_num, char* first_word, Node **curr_mcr)
{
  char* mcr_name = NULL;

  line_num++; /*todo add line error*/


  if (first_word == NULL || IS_COMMENT (first_word)) {
    return TRUE;
  }
  else if (IS_MCR_DEF (first_word)) {
    mcr_name = strtok (NULL, DELIM);
    *curr_mcr = addMcr (mcr_list, mcr_name);
    if (*curr_mcr == NULL) {
      return FALSE; /* memory error or non-valid name for the mcr */
    }
  }
  else if (IS_MCR_END (first_word)) {
    *curr_mcr = NULL;
  }
  else if (*curr_mcr) { /* inside a mcr content */
    add_content ((*curr_mcr)->data, line);
  }
  else { /* regular line */
    p_writeLine (output, line, mcr_list, first_word);
  }
  return TRUE;
}

Node *addMcr (LinkedList *mcr_list, char *mcr_name)
{
  Node *mcr_node = NULL;
  if (!isValidMcr (mcr_list, mcr_name)) {
    return NULL; /*not valid */
  }
  mcr_node = createNode (mcr_list, mcr_name, NULL);
  if (!mcr_node) {
    return NULL; /* memory error */
  }
  appendSorted (mcr_list, mcr_node);
  return mcr_node;
}

int isValidMcr (LinkedList *macro_list, char *macro_name)
{
  /* macro does not have a name */
  if (macro_name == NULL) {
    return FALSE;
  }
  /* cant use this name for macro */
  if (isSavedWord (macro_name)) {
    return FALSE;
  }
  /*name contain chars thar are not numbers or ABC */
  /*todo check if needed */
  if (!isValidName (macro_name)) {
    return FALSE;
  }
  /*already exist macro; */
  if (findNode (macro_list, macro_name)) {
    return FALSE;
  }
  return TRUE;
}

void p_writeLine (FILE *output, char *line, LinkedList *mcr_list,
                  char *first_word)
{
  /* checks if it's a mcr, and print its content */
  Node *mcr_node = findNode (mcr_list, first_word);
  if (mcr_node != NULL) {
    mcrData *mcr_data = (mcrData *) mcr_node->data;
    fprintf (output, "%s", mcr_data->content);
  }
    /* if it not a mcr, print the line */
  else {
    fprintf (output, "%s", line);
  }
}

/* *************************************************
* ...Function to handle the macro's linked list ...
***************************************************/

void *init_mcrData (const void *data)
{
  mcrData *new_data = (mcrData *) malloc (sizeof (mcrData));
  if (!new_data) {
    return NULL;
  }
  new_data->content = (char *) malloc (sizeof (char) * MAX_LINE_SIZE);
  if (!new_data->content) {
    free (new_data);
    return NULL;
  }
  RESET_STRING(new_data->content);
  new_data->total = 0;
  new_data->capacity = MAX_LINE_SIZE;

  (void) data; /*todo becouse I dont use the param*/
  return new_data;
}

int add_content (mcrData *macro_data, char *content)
{
/*  static int i = 1; */
  char *tmp = NULL;
  size_t new_capacity;
  size_t new_total = macro_data->total + strlen(content) + 1;

  /* resize content array if needed */
  if (new_total > macro_data->capacity) {
    new_capacity = 2 * new_total;
    tmp = (char *) realloc (macro_data->content, (new_capacity
                                                  * sizeof (char)));
    if (!tmp) {
      return FALSE;
    }
    macro_data->content = tmp;
    macro_data->capacity = new_capacity;
  }
  strcat(macro_data->content, content);
  macro_data->total = new_total;
/*  printf ("\n%d) curr mcr content is:\n %s", i++, macro_data->content); */
  return TRUE;
}

void print_mcrData (const char *word, const void *data, FILE *pf)
{
  mcrData *macro_data = (mcrData *) data;

  fprintf (pf, "%s:\n", word);
  fprintf (pf, "%s", macro_data->content);
  fprintf (pf, "-------------------------------\n");

}

void free_mcrData (void *data)
{
  mcrData *macro_data = (mcrData *) data;
  free (macro_data->content);
  free (macro_data);
}
