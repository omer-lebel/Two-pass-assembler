#include "preAssembler.h"
#include "utils/errors.h"

#define IS_COMMENT(s) ((s)[0] == ';')

exit_code preAssembler (char *file_name, FILE *input, FILE *output,
                        Bool *error_flag)
{
  Node *curr_mcr = NULL;
  exit_code res = SUCCESS;
  Bool overflow = FALSE;
  char prefix[MAX_LINE_LEN] = "", token[MAX_LINE_LEN] = "", postfix[MAX_LINE_LEN] = "";
  LineParts line_part;
  LinkedList *mcr_list = createList (init_mcrData, print_mcrData, free_mcrData);

  if (!mcr_list) {
    return MEMORY_ERROR;
  }
  init_line_parts (&line_part, prefix, token, postfix);
  line_part.file = file_name;

  while (res == SUCCESS && get_line (input, line_part.line,
                                     MAX_LINE_LEN, &overflow)) {
    restart_line_parts (&line_part);
    res = p_processLine (output, mcr_list, &line_part, &curr_mcr);
    if (overflow) {
      raiseError (line_length_exceeded_err, &line_part);
      *error_flag += ERROR;
    }
  }

  /* check that mcr flag is off at EOF */
  if (res == SUCCESS && curr_mcr != NULL) {
    raiseError (eof_in_macro_definition_err, &line_part);
    res = ERROR;
  }

  freeList (mcr_list);
  return res;
}

exit_code
p_processLine (FILE *output, LinkedList *mcr_list, LineParts *line,
               Node **curr_mcr)
{
  exit_code res = SUCCESS;
  char first_word[MAX_LINE_LEN];
  lineTok (line);
  strcpy (first_word, line->token);

  if (IS_EMPTY(first_word) || IS_COMMENT (first_word)) {
    fputc ('\n', output);
  }
  else if (strcmp (first_word, "mcr") == 0) { /* mcr start */
    res = mcr_handler (mcr_list, curr_mcr, line);
  }
  else if (strcmp (first_word, "endmcr") == 0) { /* mcr end */
    res = endmcr_handler (curr_mcr, line);
  }
  else if (*curr_mcr) { /* mcr content */
    lineToPostfix (line);
    res = add_content ((*curr_mcr)->data, line->postfix);
  }
  else { /* regular line */
    res = write_to_am_file (output, line, mcr_list);
  }
  return res;
}

Bool extraneous_text (LineParts *line)
{
  if (!IS_EMPTY(line->postfix)) {
    lineTok (line);
    raiseError (extraneous_text_err, line);
    return TRUE;
  }
  return FALSE;
}

exit_code mcr_handler (LinkedList *mcr_list, Node **mcr_node, LineParts *line)
{
  char *mcr_name;
  trim_end (line->postfix);

  if (*mcr_node != NULL) {
    raiseError (nested_macro_definition_err, line);
    return ERROR;
  }
  lineTok (line);
  mcr_name = line->token;

  if (!isValidMcr (mcr_list, line)) {
    return ERROR;
  }
  if (extraneous_text (line)) { /* mcr MCR xxx */
    return ERROR;
  }
  *mcr_node = createNode (mcr_list, mcr_name, NULL);
  if (!(*mcr_node)) {
    return MEMORY_ERROR;
  }
  appendSorted (mcr_list, *mcr_node);
  return SUCCESS;
}

exit_code endmcr_handler (Node **curr_mcr, LineParts *line)
{
  trim_end (line->postfix);
  if (!(*curr_mcr)) {
    raiseError (unexpected_endmcr_err, line);
    return ERROR;
  }
  else if (extraneous_text (line)) {
    return ERROR;
  }
  *curr_mcr = NULL;
  return SUCCESS;
}

exit_code write_to_am_file (FILE *am_file, LineParts *line,
                            LinkedList *mcr_list)
{
  mcrData *mcr_data;
  /* checks if it's a mcr, and print its content */
  Node *mcr_node = findNode (mcr_list, line->token);
  if (mcr_node) {
    trim_end (line->postfix);
    if (extraneous_text (line)) {
      return ERROR;
    }
    mcr_data = (mcrData *) mcr_node->data;
    fprintf (am_file, "%s", mcr_data->content);
  }
    /* if it not a mcr, print the line */
  else {
    lineToPostfix (line);
    fprintf (am_file, "%s", line->postfix);
  }
  return SUCCESS;
}

Bool isValidMcr (LinkedList *macro_list, LineParts *line)
{
  char *mcr_name = line->token;
  /* macro does not have a name */
  if (IS_EMPTY(mcr_name)) {
    raiseError (empty_macro_declaration_err, line);
    return FALSE;
  }

  if (!isalpha(mcr_name[0])) {
    raiseError (starts_with_non_alphabetic_err, line);
    return FALSE;
  }
  if (isSavedWord (mcr_name)) {
    raiseError (reserved_keyword_used_err, line);
    return FALSE;
  }

  /*already exist macro; */
  if (findNode (macro_list, mcr_name)) {
    raiseError (redeclaration_err, line);
    return FALSE;
  }
  return TRUE;
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
  new_data->content = (char *) malloc (sizeof (char) * MAX_LINE_LEN);
  if (!new_data->content) {
    free (new_data);
    return NULL;
  }
  RESET_STR(new_data->content);
  new_data->total = 0;
  new_data->capacity = MAX_LINE_LEN;

  (void) data; /* todo because I don't use the param */
  return new_data;
}

exit_code add_content (mcrData *macro_data, char *content)
{
  char *tmp = NULL;
  size_t new_capacity;
  size_t new_total = macro_data->total + strlen (content) + 1;

  /* resize content array if needed */
  if (new_total > macro_data->capacity) {
    new_capacity = 2 * new_total;
    tmp = (char *) realloc (macro_data->content, (new_capacity
                                                  * sizeof (char)));
    if (!tmp) {
      return MEMORY_ERROR;
    }
    macro_data->content = tmp;
    macro_data->capacity = new_capacity;
  }
  strcat (macro_data->content, content);
  macro_data->total = new_total;
  return SUCCESS;
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
