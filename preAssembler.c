/* ------------------------------- includes ------------------------------- */
#include "preAssembler.h"
#include "utils/text.h"
#include "utils/linkedList.h"
#include "utils/errors.h"
/* ------------------------------- defines -------------------------------- */
#define IS_COMMENT(s) ((s)[0] == ';')
/* ------------------------------- typedef -------------------------------- */
/**
 * @typedef mcrData
 * Data structure representing the content of a macro in the linked list.
 */
typedef struct mcrData
{
    char *content;
    size_t total;
    size_t capacity;
} mcrData;

/* ---------------------- helper function declaration ---------------------- */

/* function of the macro list */
void *init_mcrData (const void *data);
exit_code add_content (mcrData *macro_data, char *content);
void print_mcrData (const char *word, const void *data, FILE *pf);
void free_mcrData (void *data);

/* function of pre process analyze */
exit_code   pre_process_line  (FILE *output, LinkedList *mcr_list, LineParts *line, Node **curr_mcr);
exit_code   mcr_handler       (LinkedList *mcr_list, Node **mcr_node, LineParts *line);
exit_code   endmcr_handler    (Node **curr_mcr, LineParts *line);
exit_code   write_to_am_file  (FILE *am_file, LineParts *line, LinkedList *mcr_list);
exit_code   handle_end        (char *file_name, Node *curr_mcr, exit_code res,
                                LineParts *line_part,  LinkedList *mcr_list);
Bool        isValidMcr        (LinkedList *macro_list, LineParts *line);
Bool        extraneous_text   (LineParts *line);

/* ------------------------------------------------------------------------- */

exit_code preAssembler (char *file_name, FILE *input, FILE *output, int *error)
{
  Node *curr_mcr = NULL;
  exit_code res = SUCCESS;
  Bool overflow = FALSE;
  char prefix[MAX_LINE_LEN] = "", token[MAX_LINE_LEN] = "", postfix[MAX_LINE_LEN] = "";
  LineParts line_part;
  LinkedList *mcr_list = create_list (init_mcrData, print_mcrData, free_mcrData);

  if (!mcr_list) {
    return MEMORY_ERROR;
  }
  init_line_parts (&line_part, prefix, token, postfix);
  line_part.file = file_name;

  while (res == SUCCESS && get_line (input, line_part.line,
                                     MAX_LINE_LEN, &overflow)) {
    restart_line_parts (&line_part);
    if (overflow) {
      raise_error (line_length_exceeded_err, &line_part);
      (*error)++;
    }
    res = pre_process_line (output, mcr_list, &line_part, &curr_mcr);
  }

  return handle_end (file_name, curr_mcr, res, &line_part, mcr_list);
}

/**
 * @brief Processes a line during the pre-processing phase of the assembler.
 *
 * This function processes a line of the input file during the
 * pre-processing phase of the assembler.
 * It determines whether the line is a macro definition, a macro end, macro
 * content, or a regular line, and calls the appropriate handler function
 * accordingly.
 *
 * @param output    The output file to store the processed content.
 * @param mcr_list  Pointer to the linked list of macros.
 * @param line      Pointer to the LineParts structure containing information about the line.
 * @param curr_mcr  Pointer to current macro if inside a definition, otherwise NULL.
 * @return exit_code
 */
exit_code pre_process_line (FILE *output, LinkedList *mcr_list, LineParts *line,
                  Node **curr_mcr)
{
  exit_code res = SUCCESS;
  char first_word[MAX_LINE_LEN];
  lineTok (line);
  strcpy (first_word, line->token);

  if (IS_EMPTY(first_word) || IS_COMMENT (first_word)) {
    if (*curr_mcr == NULL){ /*not inside macro def */
      fputc ('\n', output);
    }
      }
  else if (strcmp (first_word, "mcr") == 0) { /* mcr start */
    res = mcr_handler (mcr_list, curr_mcr, line);
  }
  else if (strcmp (first_word, "endmcr") == 0) { /* mcr end */
    res = endmcr_handler (curr_mcr, line);
  }
  else if (*curr_mcr) { /* mcr content */
    line_to_postfix (line);
    res = add_content ((*curr_mcr)->data, line->postfix);
  }
  else { /* regular line */
    res = write_to_am_file (output, line, mcr_list);
  }
  return res;
}

/**
 * @brief Handles line of a macro definition.
 *
 * This function validating the macro name and creating a new node
 * in the macro linked list to store the macro content.
 *
 * @param mcr_list  Pointer to the linked list of macros.
 * @param mcr_node  Pointer to the current macro node.
 * @param line      Pointer to the LineParts structure containing information about the line.
 * @return exit_code
 */
exit_code mcr_handler (LinkedList *mcr_list, Node **mcr_node, LineParts *line)
{
  char *mcr_name;
  trim_end (line->postfix);

  if (*mcr_node != NULL) {
    raise_error (nested_macro_definition_err, line);
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
  *mcr_node = create_node (mcr_list, mcr_name, NULL);
  if (!(*mcr_node)) {
    return MEMORY_ERROR;
  }
  append_sorted (mcr_list, *mcr_node);
  return SUCCESS;
}

/**
 * @brief Handles the line of end of a macro definition.
 *
 * This function handles the end of a macro definition, validating the end
 * of the macro and resetting the current macro node pointer.
 *
 * @param curr_mcr  Pointer to the current macro node.
 * @param line      Pointer to the LineParts structure containing information about the line.
 * @return exit_code
 */
exit_code endmcr_handler (Node **curr_mcr, LineParts *line)
{
  trim_end (line->postfix);
  if (!(*curr_mcr)) {
    raise_error (unexpected_endmcr_err, line);
    return ERROR;
  }
  else if (extraneous_text (line)) {
    return ERROR;
  }
  *curr_mcr = NULL;
  return SUCCESS;
}

/**
 * @brief Writes the line to the output file (.am file)
 *
 * This function writes the processed line to the output file.
 * If the line corresponds to a macro, the macro content is written instead
 * of the original line.
 *
 * @param am_file   Pointer to the output file.
 * @param line      Pointer to the LineParts structure containing information about the line.
 * @param mcr_list  Pointer to the linked list of macros.
 * @return exit_code
 */
exit_code write_to_am_file (FILE *am_file, LineParts *line,
                            LinkedList *mcr_list)
{
  mcrData *mcr_data;
  /* checks if it's a mcr, and print its content */
  Node *mcr_node = find_node (mcr_list, line->token);
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
    line_to_postfix (line);
    fprintf (am_file, "%s", line->postfix);
  }
  return SUCCESS;
}

/**
 * @brief Checks if a macro name is valid.
 *
 * This function checks if a macro name is valid, which means:
 * - Must have a name (only 'mcr' is not enough). <br>
 * - Cannot be a reserved word of the language. <br>
 * - start with alphabetic char <br>
 * - Can contain only letters, numbers, and .<br>
 *
 * @param macro_list    Pointer to the linked list of macros.
 * @param line          Pointer to the LineParts structure containing information about the line.
 * @return TRUE if the macro name is valid, FALSE otherwise.
 */
Bool isValidMcr (LinkedList *macro_list, LineParts *line)
{
  char *mcr_name = line->token;
  /* macro does not have a name */
  if (IS_EMPTY(mcr_name)) {
    raise_error (empty_macro_declaration_err, line);
    return FALSE;
  }

  if (!isalpha(mcr_name[0])) {
    raise_error (starts_with_non_alphabetic_err, line);
    return FALSE;
  }
  if (is_saved_word (mcr_name)) {
    raise_error (reserved_keyword_used_err, line);
    return FALSE;
  }

  /*already exist macro; */
  if (find_node (macro_list, mcr_name)) {
    raise_error (redeclaration_err, line);
    return FALSE;
  }
  return TRUE;
}

/**
 * @brief Checks for extraneous text
 *
 * This function checks for extraneous text after
 * a macro definition (mcr MCR_NAME ___) and after end of a macro definition
 * (endmcr ___) and raises an error if any is found.
 *
 * @param line  Pointer to the LineParts structure containing information about the line.
 * @return TRUE if extraneous text is found, FALSE otherwise.
 */
Bool extraneous_text (LineParts *line)
{
  if (!IS_EMPTY(line->postfix)) {
    lineTok (line);
    raise_error (extraneous_text_err, line);
    return TRUE;
  }
  return FALSE;
}

/**
 * @brief Handles preprocessing completion
 *
 * This function checks for errors and incomplete macro definitions.
 * If found fatal errors during the pre processor it delete the .am file
 *
 * @param file_name     The name of the input file.
 * @param curr_mcr      Pointer to the current macro being processed.
 * @param res           The preprocessing result.
 * @param line_part     Pointer to line parts for error reporting.
 * @param mcr_list      Pointer to the macro definitions list.
 * @return exit code
 */
exit_code handle_end (char *file_name, Node *curr_mcr, exit_code res,
                      LineParts *line_part, LinkedList *mcr_list)
{/* check that mcr flag is off at EOF */
  if (res == SUCCESS && curr_mcr != NULL) {
    raise_error (eof_in_macro_definition_err, line_part);
    res = ERROR;
  }


  file_name[strlen (file_name) - 1] = 'm'; /* change the file extension from .as to .am */
  if (res == ERROR) {
    remove (file_name);
  }
  else {
    printf (" -- %s created \n", file_name);
  }

#ifdef DEBUG_PRE
  if (res != MEMORY_ERROR){
    print_list (mcr_list, stdout);
  }
#endif

  free_list (mcr_list);
  return res;
}

/* =========================================================================
 *             Function to handle the macro's linked list
 * ========================================================================= */


/**
 * @brief Initializes macro data structure.
 *
 * @param data  Pointer to the data to initialize.
 * @return Pointer to the initialized macro data structure.
 */
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

  (void) data; /* done because the param is unused  */
  return new_data;
}

/**
 * @brief Adds content to a macro.
 *
 * @param macro_data    Pointer to the macro data structure.
 * @param content       Content to add to the macro.
 * @return exit code
 */
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

/**
 * @brief Prints the content of a macro.
 *
 * @param word  The macro name.
 * @param data  Pointer to the macro data structure.
 * @param pf    Pointer to the file to print to.
 */
void print_mcrData (const char *word, const void *data, FILE *pf)
{
  mcrData *macro_data = (mcrData *) data;

  fprintf (pf, "%s:\n", word);
  fprintf (pf, "%s", macro_data->content);
  fprintf (pf, "-------------------------------\n");

}

/**
 * @brief Frees memory allocated for macro data structure.
 *
 * @param data Pointer to the macro data structure.
 */
void free_mcrData (void *data)
{
  mcrData *macro_data = (mcrData *) data;
  free (macro_data->content);
  free (macro_data);
}
