/*
 Created by OMER on 1/15/2024.
*/

#include "firstPass.h"

#include "utils/errors.h"
#include "utils/text.h"
#include "utils/vector.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/dataSeg.h"
#include "fileStructures/codeSeg.h"
#include "fileStructures/entryLines.h"
#include "firstAnalysis.h"

/*  --------------- first pass helpers */
exit_code
init_first_pass (file_analyze *f, char *file_name, LineInfo *line_info,
                 LineParts *line_part, char *label);
exit_code first_process (file_analyze *f, LineInfo *line_info);
exit_code first_pass_with_opening_label (file_analyze *f, LineInfo *line);
Opcode get_opcode (char *token);
Bool check_overflow (LineParts *line, Bool overflow, int memory_size);

/* ---------------------------------------------------------------------- */


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
      continue;
    }
    else if (IS_LABEL(line_info.parts->token)) {
      res = first_pass_with_opening_label (f, &line_info);
    }
    else {
      res = first_process (f, &line_info);
    }

    if (!overflow) {
      overflow = check_overflow (&line_parts, overflow, f->IC + f->DC);
      res = overflow ? ERROR : res;
    }
  }

#ifdef DEBUG
  display_op_list (f->op_list, stdout);
  display_data_segment (f->data_segment, stdout);
  fprintf (stdout, "------------------ before update ------------------");
  display_symbol_table (f->symbol_table, stdout);
#endif

  return res;
}

exit_code init_first_pass (file_analyze *f, char *file_name,
                           LineInfo *line_info, LineParts *line_part,
                           char *label)
{
  f->symbol_table = new_symbol_table ();
  f->data_segment = new_data_segment (&f->DC);
  f->op_list = new_op_list ();
  f->entry_list = new_entry_list ();
  if (!f->symbol_table || !f->data_segment || !f->op_list || !f->entry_list) {
    free_file_analyze (f);
    return MEMORY_ERROR;
  }

  /* line info */
  line_info->label = label;
  line_info->parts = line_part;
  line_part->file = file_name;
  return SUCCESS;
}

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
    res = op_analyze (line_info, opcode, f);
  }
  else {
    res = else_analyze (line_info);
  }

  f->error = (res == ERROR) ? f->error + 1 : f->error;
  return res;
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

exit_code first_pass_with_opening_label (file_analyze *f, LineInfo *line)
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

Bool check_overflow (LineParts *line, Bool overflow, int memory_size)
{
  if (!overflow && (memory_size >= MEMORY_IMG_SIZE)) {
    raise_error (exceeds_available_memory, line);
    return TRUE;
  }
  return overflow;
}









