/* ------------------------------- includes ------------------------------- */
#include "firstPass.h"
#include "setting.h"
#include "utils/errors.h"
#include "utils/text.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/dataSeg.h"
#include "fileStructures/codeSeg.h"
#include "fileStructures/entryLines.h"
#include "firstAnalysis.h"
/* ---------------------- helper function declaration ---------------------- */
exit_code   init_first_pass       (file_analyze *f, char *file_name,
                                   LineInfo *line_info, LineParts *line_part, char *label);
exit_code   first_process         (file_analyze *f, LineInfo *line_info);
exit_code   process_opening_label (file_analyze *f, LineInfo *line);
Opcode      get_opcode            (char *token);
Bool        check_overflow        (LineParts *line, Bool overflow, int memory_size);
/* ------------------------------------------------------------------------- */


exit_code firstPass (FILE *input_file, file_analyze *f)
{
  exit_code res;
  Bool overflow = FALSE;
  LineInfo line_info;
  LineParts line_parts;
  char prefix[MAX_LINE_LEN] = "", token[MAX_LINE_LEN] = "",
      postfix[MAX_LINE_LEN] = "", label[MAX_LINE_LEN] = "";

  init_line_parts (&line_parts, prefix, token, postfix);
  res = init_first_pass (f, f->file_name, &line_info, &line_parts, label);

  while (res != MEMORY_ERROR &&
         fgets (line_info.parts->line, MAX_LINE_LEN, input_file)) {
    restart_line_info (&line_info);
    lineTok (line_info.parts);

    if (IS_EMPTY(line_info.parts->token)) {
      res = SUCCESS; /* continue */
    }
    else if (IS_LABEL(line_info.parts->token)) {
      res = process_opening_label (f, &line_info);
    }
    else { /* no label */
      res = first_process (f, &line_info);
    }

    if (!overflow) { /* check for overflow */
      overflow = check_overflow (&line_parts, overflow, f->IC + f->DC);
      f->error = overflow ? f->error + 1 : f->error;
    }
  }

#ifdef DEBUG_FIRST_PASS
  if (res != MEMORY_ERROR){
    display_debug(f, stdout, "first");
  }
#endif

  return res;
}


/**
 * @brief Initializes the first pass analysis.
 *
 * Initializes the components required for the first pass analysis,
 * including symbol table, data segment, op list, and entry list.
 *
 * @param f             Pointer to the file analysis structure.
 * @param file_name     Name of the input file.
 * @param line_info     Pointer to the LineInfo structure.
 * @param line_part     Pointer to the LineParts structure.
 * @param label         Pointer to store the label encountered.
 * @return exit_code
 */
exit_code init_first_pass (file_analyze *f, char *file_name,
                           LineInfo *line_info, LineParts *line_part,
                           char *label)
{
  f->symbol_table = new_symbol_table ();
  f->data_segment = new_data_segment ();
  f->op_list = new_op_lines_list ();
  f->entry_list = new_entry_lines_list ();
  if (!f->symbol_table || !f->data_segment || !f->op_list || !f->entry_list) {
    free_file_analyze (f);
    return MEMORY_ERROR;
  }

  /* init line info */
  line_info->label = label;
  line_info->parts = line_part;
  line_part->file = file_name;
  return SUCCESS;
}

/**
 * @brief Processes a line in the first pass.
 *
 * Processes a line from the input file during the first pass analysis.
 * Determines the type of the line and delegates further processing
 * accordingly.
 *
 * @param f             Pointer to the file analysis structure.
 * @param line_info     Pointer to the LineInfo structure.
 * @return exit_code
 *
 * @note Lines starting with a label are first processed by
 *       process_opening_label() function to handle label
 *       declaration and then passed to this function for further processing.
 *       This function expects the current token to contain instructions or
 *       directives that determine the line type.
 */
exit_code first_process (file_analyze *f, LineInfo *line_info)
{
  exit_code res;
  Opcode opcode;
  char *token = line_info->parts->token;

  if (strcmp (".define", token) == 0) {
    res = define_analyze (line_info, f->symbol_table);
  }
  else if (strcmp (".entry", token) == 0) {
    res = ent_analyze (line_info, f->symbol_table, f->entry_list);
  }
  else if (strcmp (".extern", token) == 0) {
    res = ext_analyze (line_info, f->symbol_table);
  }
  else if (strcmp (".string", token) == 0) {
    res = str_analyze (line_info, f->symbol_table, f->data_segment, &(f->DC));
  }
  else if (strcmp (".data", token) == 0) {
    res = data_analyze (line_info, f->symbol_table, f->data_segment, &(f->DC));
  }
  else if ((opcode = get_opcode (token)) != NO_OPCODE) {
    res = op_analyze (line_info, opcode, f->symbol_table, f->op_list, &(f->IC));
  }
  else {
    res = else_analyze (line_info);
  }

  f->error = (res == ERROR) ? f->error + 1 : f->error;
  return res;
}

/**
 * @brief Gets the opcode from a token.
 *
 * Checks if the token corresponds to an opcode and returns the corresponding
 * opcode if found.
 * This function uses the global array op_names declared in setting.h.
 *
 * @param token     The token to check.
 * @return Opcode Returns the corresponding opcode if found,
 *         NO_OPCODE otherwise.
 */
Opcode get_opcode (char *token)
{
  int i;
  for (i = 0; i < NUM_OF_OP; ++i) {
    if (strcmp (token, operation_names[i]) == 0) {
      return i;
    }
  }
  return NO_OPCODE;
}

/**
 * @brief Processes a line with an opening label.
 *
 * Processes a line that starts with a label during the first pass analysis.
 * Checks that the label has valid syntax and verifies if it's a new symbol
 * or an unresolved one. If the syntax is valid and the symbol is new or
 * unresolved, the line is sent to the first_process() function for further
 * analysis.
 *
 * @param f     Pointer to the file analysis structure.
 * @param line  Pointer to the LineInfo structure.
 * @return exit_code
 */
exit_code process_opening_label (file_analyze *f, LineInfo *line)
{
  Symbol *symbol;
  Symbol_Data *symbol_data;

  strcpy (line->label, line->parts->token);
  REMOVE_LAST_CHAR(line->label); /* remove ":" */

  if (!is_valid_identifier (line->parts, line->label, TRUE)) {
    return ERROR;
  }

  /* if the symbol already exist and resolved - error */
  if ((symbol = find_symbol (f->symbol_table, line->label))) {
    symbol_data = (Symbol_Data *) symbol->data;

    if (symbol_data->type != UNRESOLVED_USAGE
        && symbol_data->type != UNRESOLVED_ENTRY
        && symbol_data->type != UNRESOLVED_ENTRY_USAGE) {

      raise_error (redeclaration_err, line->parts);
      return ERROR;
    }
  }
  lineTok (line->parts);
  return first_process (f, line);
}

/**
 * @brief Checks for overflow in memory size.
 *
 * Checks if the current memory size exceeds the available memory size
 * and raises an error if so.
 *
 * @param line          Pointer to the LineParts structure.
 * @param overflow      Flag indicating if overflow has already occurred.
 * @param memory_size   Current memory size.
 * @return Bool Returns TRUE if overflow occurs, FALSE otherwise.
 *
 * @note Error of exceeds_available_memory will be shown only in the first
 * line that exceeds the memory size.
 */
Bool check_overflow (LineParts *line, Bool overflow, int memory_size)
{
  if (!overflow && (memory_size >= MEMORY_IMG_SIZE)) {
    raise_error (exceeds_available_memory, line);
    return TRUE;
  }
  return overflow;
}









