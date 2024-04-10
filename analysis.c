#include "analysis.h"

/* =======================================================================
 *                      Syntax analysis helpers
 * ======================================================================= */

Bool is_reg (const char *token, int *reg_num)
{
  if (token[0] == 'r' && IN_REG_BOUND(token[1]) && token[2] == '\0') {
    *reg_num = CHAR_TO_INT(token[1]);
    return TRUE;
  }
  return FALSE;
}

Bool is_str (LineParts *line)
{
  char *str = line->token;
  size_t len = strlen (line->token);

  if (IS_EMPTY (str)) { /* .string _ */
    raise_error (empty_string_declaration_err, line);
    return FALSE;
  }
  if (str[0] != '"') { /* .string a" */
    raise_error (missing_opening_quote_err, line);
    return FALSE;
  }
  if (len == 1 || str[len - 1] != '"') { /*.string " || .string "abc   */
    raise_error (missing_terminating_quote_err, line);
    return FALSE;
  }
  return TRUE;
}

Bool is_int (char *token, int *imm)
{
  char *end_ptr = NULL;
  *imm = (int) strtol (token, &end_ptr, 10);
  return IS_EMPTY(end_ptr);
}

Bool
is_define_constant (LineInfo *line, char *token, Symbol_Table *symbol_table,
                    int *imm)
{
  Symbol_Data *symbol_data;
  Symbol *symbol = find_symbol (symbol_table, token);
  if (symbol) {
    symbol_data = (Symbol_Data *) symbol->data;
    if (symbol_data->type == DEFINE) {
      *imm = symbol_data->val;
      return TRUE;
    }
    else { /* mov r0, #STR | mov r0, LABEL[STR] */
      raise_error (label_instead_of_imm_err, line->parts);
      return FALSE;
    }
  }
  /* mov r0, #zzz | mov L[zzz], r0 | .data 1,zzz (when 'zzz' in undefined) */
  if (is_valid_identifier (line->parts, token, FALSE)) {
    raise_error (undeclared_err, line->parts);
  }

    /* mov r0, L[] */
  else if ((*token == ',') || IS_CLOSE_INDEX_SIGN(token)) {
    raise_error (expected_int_before_bracket, line->parts);
  }
    /* mov r0, #3!a |  mov r0, L[***] | .data 1,2, $ */
  else {
    raise_error (invalid_imm_err, line->parts);
  }
  return FALSE;
}

Bool is_imm (LineInfo *line, char *token, Symbol_Table *table, int *imm)
{
  if (IS_EMPTY(token) && line->type != data_line) {
    /* relevant only for # sign. exp: mov r0, #_ */
    raise_error (expected_imm_after_sign_err, line->parts);
    return FALSE;
  }
  if (is_int (token, imm)) {
    if (*imm >= MIN_INT_MACHINE && *imm <= MAX_INT_MACHINE) {
      return TRUE;
    }
    raise_error (exceeds_integer_bounds, line->parts);
    return FALSE;
  }
  /*else check if it's a define constant */
  return is_define_constant (line, token, table, imm);
}

Bool is_valid_identifier (LineParts *line, char *name, Bool print_err)
{
  if (!isalpha(name[0])) {
    if (print_err) {
      raise_error (starts_with_non_alphabetic_err, line);
    }
    return FALSE;
  }
  if (!is_alpha_numeric (name)) {
    if (print_err) {
      raise_error (contain_non_alphanumeric_err, line);
    }
    return FALSE;
  }
  if (is_saved_word (name)) {
    if (print_err) {
      raise_error (reserved_keyword_used_err, line);
    }
    return FALSE;
  }
  return TRUE;
}

Bool is_symbol (LineInfo *line, char *name, Symbol_Table *symbol_table,
                Operand *operand)
{
  Symbol *symbol;
  Symbol_Data *symbol_data;
  if (!is_valid_identifier (line->parts, name, FALSE)) {
    return FALSE;
  }

  if ((symbol = find_symbol (symbol_table, name))) {
    symbol_data = (Symbol_Data *) symbol->data;
    if (symbol_data->type == DEFINE) {
      return FALSE;
    }
    /* if found an existing symbol: update the operand info */
    operand->info.symInx.symbol = symbol;
    operand->info.symInx.found = TRUE;

    if (symbol_data->type == UNRESOLVED_ENTRY) {
      symbol_data->type = UNRESOLVED_ENTRY_USAGE;
      symbol_table->unresolved_symbols_count++;
    }
  }
  else { /*label is not exist yet */
    strcpy (operand->info.symInx.symbol, name);
    operand->info.symInx.found = FALSE;
  }
  return TRUE;

}

Bool is_index (LineInfo *line, char *name, Symbol_Table *table,
               Operand *operand)
{
  char *token = line->parts->token;

  if (!is_symbol (line, name, table, operand)) {
    return FALSE;
  }

  lineTok (line->parts); /* check to offset */
  if (!is_imm (line, token, table, &operand->info.symInx.offset)) {
    return FALSE;
  }

  /* check ] sign */
  lineTok (line->parts);
  return is_closing_index_sign (line);
}

Bool is_closing_index_sign (LineInfo *line)
{
  char *token = line->parts->token;
  if (IS_EMPTY(token)) { /* mov r0, L[1 */
    raise_error (expected_bracket_got_nothing_err, line->parts);
    return FALSE;
  }
  if (!IS_CLOSE_INDEX_SIGN(token)) { /* mov r0, L[1 k] */
    raise_error (expected_bracket_before_expression_err, line->parts);
    return FALSE;
  }
  return TRUE;
}

/* =======================================================================
 *                      Semantic analysis helpers
 * ======================================================================= */

Bool valid_add_mode (LineInfo *line, Operand *operand, addr_mode_flag add_mode)
{
  if (operand->param_types & add_mode) {
    switch (add_mode) {
      case b_imm:
        operand->add_mode = IMM_ADD;
        break;
      case b_symbol:
        operand->add_mode = DIRECT_ADD;
        break;
      case b_index:
        operand->add_mode = INDEX_ADD;
        break;
      case b_reg:
        operand->add_mode = REG_ADD;
        break;
    }
    return TRUE;
  }

  raise_error (invalid_addressing_mode_err, line->parts);
  return FALSE;
}

Bool valid_entry_label (LineInfo *line, Symbol *symbol)
{
  Symbol_Data *symbol_data = (Symbol_Data *) symbol->data;

  if (symbol_data->type == DEFINE || symbol_data->type == EXTERN) {
    raise_error (redeclaration_err, line->parts);
    return FALSE;
  }

  /* else, label can be entry label (dada, code, unresolved) */
  if (symbol_data->isEntry) {
    raise_warning (duplicate_declaration_warning, line->parts);
  }
  line->info.ext_ent.name = symbol;
  line->info.ext_ent.found = TRUE;
  return TRUE;
}

Bool valid_extern_label (LineInfo *line, Symbol *symbol)
{
  Symbol_Data *symbol_data = (Symbol_Data *) symbol->data;

  if (symbol_data->type == EXTERN) {
    raise_warning (duplicate_declaration_warning, line->parts);
    line->info.ext_ent.name = symbol;
    line->info.ext_ent.found = TRUE;
    return TRUE;
  }

  if (symbol_data->type == UNRESOLVED_USAGE) {
    line->info.ext_ent.name = symbol;
    line->info.ext_ent.found = TRUE;
    return TRUE;
  }

  /* else, not valid. including: code, data, entry, define, unresolved */
  raise_error (redeclaration_err, line->parts);
  return FALSE;
}

Bool valid_define_symbol (LineInfo *line, Symbol *symbol)
{
  Symbol_Data *symbol_data = (Symbol_Data *) symbol->data;

  if (symbol_data->type == DEFINE) {
    line->info.define.name = symbol;
    line->info.define.found = TRUE;
    return TRUE;
  }
  raise_error (redeclaration_err, line->parts);
  return FALSE;
}


/* =======================================================================
 *                      LineInfo management
 * ======================================================================= */

/* save the buffers of lineParts and label, but set everything to 0 */
void restart_line_info (LineInfo *line)
{
  LineParts *tmp_part = line->parts;
  char *tmp_label = line->label;

  memset (line, 0, sizeof (LineInfo));

  line->label = tmp_label;
  NULL_TERMINATE(line->label, 0);

  line->parts = tmp_part;
  restart_line_parts (line->parts);
  trim_end (line->parts->postfix);
}

void print_data (int *arr, unsigned len)
{
  unsigned int i;
  printf ("data: " "%d", arr[0]);
  for (i = 1; i < len; ++i) {
    printf (", %d", arr[i]);
  }
  printf ("\n");
}

void display_line_info (LineInfo *line)
{
  printf ("%s:%-2d ", line->parts->file, line->parts->num);

  switch (line->type) {
    case str_line:
      printf ("string: %s\n", line->info.str.content);
      break;
    case data_line:
      print_data (line->info.data.arr, line->info.data.len);
      break;
    case ext_line:
      printf ("extern: %s\n",
              ((Symbol *) line->info.ext_ent.name)->word);
      break;
    case ent_line:
      printf ("entry: %s\n",
              ((Symbol *) line->info.ext_ent.name)->word);
      break;
    case op_line:
      display_op_line (line->info.op, stdout);
      break;
    case def_line:
      printf ("define: %s = %d\n",
              ((Symbol *) line->info.define.name)->word,
              line->info.define.val);
      break;
  }
}



