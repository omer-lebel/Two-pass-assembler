/*
 Created by OMER on 1/23/2024.
*/


#include "fsm.h"
#include "utils/errors.h"


#define IS_COMMA(s) ((s) == ',')
#define IS_IMM_SIGN(s) ((s) == '#')
#define IS_OPEN_INDEX_SIGN(s) (s == '[')
#define IS_CLOSE_INDEX_SIGN(s) (strcmp (s, "]") == 0)
#define CHAR_TO_INT(c) ((c) - '0')
#define IN_REG_BOUND(s) ((s) >= '0' && (s) <= '7')

/* 2 operators map (mov, cmp, add, sub, lea) */
transition two_operand_map[] = {
    {SRC_STATE,        &src_handler,        COMA_STATE},
    {COMA_STATE,       &comma_handler,      TARGET_STATE},
    {TARGET_STATE,     &target_handler,     EXTRA_TEXT_STATE},
    {EXTRA_TEXT_STATE, &extra_text_handler, END_STATE},
};

/* 1 operators map (not, clr, inc, dec, jmp, bne, red, prn, jsr)*/
transition one_operand_map[] = {
    {TARGET_STATE,     &target_handler,     EXTRA_TEXT_STATE},
    {EXTRA_TEXT_STATE, &extra_text_handler, END_STATE},
};

/* no operators map (rts, hlt)*/
transition zero_operand_map[] = {
    {EXTRA_TEXT_STATE, &extra_text_handler, END_STATE},
};

/* string */
transition str_map[] = {
    {STRING_STATE, &str_handler, END_STATE},
};

/* .data */
transition data_map[] = {
    {IMM_STATE,  &imm_handler,   COMA_STATE},
    {COMA_STATE, &comma_handler, IMM_STATE},
};

/* define */
transition define_map[] = {
    {IDENTIFIER_STATE, &identifier_handler, EQUAL_STATE},
    {EQUAL_STATE,      &equal_handler,      INT_STATE},
    {INT_STATE,        &int_handler,        EXTRA_TEXT_STATE},
    {EXTRA_TEXT_STATE, &extra_text_handler, END_STATE},
};

/* extern and entry */
transition ext_ent_map[] = {
    {IDENTIFIER_STATE, &identifier_handler, EXTRA_TEXT_STATE},
    {EXTRA_TEXT_STATE, &extra_text_handler, END_STATE},
};

transition *get_map (LineInfo *line)
{
  Opcode o;
  switch (line->type_l) {
    case str_l:
      return str_map;
    case data_l:
      return data_map;
    case def_l:
      return define_map;
    case ext_l:
    case ent_l:
      return ext_ent_map;
    case op_l:
      o = line->info.op->opcode;
      if (o == MOV || o == CMP || o == ADD || o == SUB || o == LEA) {
        return two_operand_map;
      }
      if (o == NOT || o == CLR || o == INC || o == DEC || o == JMP || o == BNE
          || o == RED || o == PRN || o == JSR) {
        return one_operand_map;
      }
      if (o == RTS || o == HLT) {
        return zero_operand_map;
      }
  }
  return NULL;
}

Bool is_reg (const char *str, int *reg_num)
{
  if (str[0] == 'r' && IN_REG_BOUND(str[1]) && str[2] == '\0') {
    *reg_num = CHAR_TO_INT(str[1]);
    return TRUE;
  }
  return FALSE;
}

Bool is_int (char *token, int *imm)
{
  char *end_ptr = NULL;
  *imm = (int) strtol (token, &end_ptr, 10);
  return IS_EMPTY(end_ptr);
}

Bool is_define (LineInfo *line, char *token, Symbol_Table *symbol_table,
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
  else if (IS_COMMA(*token) || IS_CLOSE_INDEX_SIGN(token)) {
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
  if (IS_EMPTY(token) && line->type_l != data_l) {
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
  return is_define (line, token, table, imm);
}

/* put in symbol: the symbol name or pointer to the symbol */
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
    operand->info.symInx.symbol = symbol;
    operand->info.symInx.found = TRUE;
    if (symbol_data->type == UNRESOLVED_ENTRY) {
      symbol_data->type = UNRESOLVED_ENTRY_USAGE;
      symbol_table->unresolved_usage_count++;
    }
  }
  else { /*label is not exist yet */
    strcpy (operand->info.symInx.symbol, name);
    operand->info.symInx.found = FALSE;
  }
  return TRUE;

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
      raise_error (contain_non_alphanumeric, line);
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

addr_mode_flag get_addressing_mode (LineInfo *line, Symbol_Table *table,
                                    Operand *operand)
{
  char *token = line->parts->token;
  char tmp[MAX_LINE_LEN];

  /*register*/
  if (is_reg (token, &operand->info.reg_num)) {
    return b_reg;
  }
  /*imm*/
  if (IS_IMM_SIGN (token[0])) {
    strcpy (tmp, token + 1); /* remove # sign */
    return is_imm (line, tmp, table, &operand->info.imm) ? b_imm : 0;
  }
  strcpy (tmp, token);
  /* index */
  if (IS_OPEN_INDEX_SIGN(token[strlen (token) - 1])) {
    REMOVE_LAST_CHAR(tmp); /* remove [ char */
    return is_index (line, tmp, table, operand) ? b_index : 0;
  }
  /*symbol*/
  if (is_symbol (line, tmp, table, operand)) {
    return b_symbol;
  }

  /* else, the operand is invalid. exp: mov r2, $%#! */
  raise_error (invalid_operand_err, line->parts);
  return 0;
}

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

state operand_handler (LineInfo *line, Symbol_Table *table,
                       Operand *operand, state next_state)
{
  addr_mode_flag add_mode;
  char *token = line->parts->token;

  /* check for missing arg (exp: mov r2, _ | mov _) */
  if (IS_EMPTY(token)) {
    raise_error (too_few_arguments_err, line->parts);
    return ERROR_STATE;
  }

  /* check for redundant comma (exp: mov, r2, r3 | mov r2,,r3) */
  if (IS_COMMA(*token)) {
    raise_error (expected_expression_before_comma_err, line->parts);
    return ERROR_STATE;
  }

  /* find the addressing mode and assign it to the appropriate operand*/
  add_mode = get_addressing_mode (line, table, operand);

  /*if couldn't find any addressing mode */
  if (add_mode == 0) {
    return ERROR_STATE;
  }

  /* check if it is a valid addressing mode */
  if (!valid_add_mode (line, operand, add_mode)) {
    return ERROR_STATE;
  }
  return next_state;
}

state src_handler (LineInfo *line, Symbol_Table *table, state next_state)
{
  return operand_handler (line, table, &line->info.op->src,
                          next_state);
}

state
comma_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state)
{
  char *token = line->parts->token;
  if (IS_COMMA(*token)) {
    return next_state;
  }

  if (!IS_EMPTY(token)) { /* mov r0 r3 | .data 1 2 */
    raise_error (expected_expression_before_comma_err, line->parts);
  }
  else { /* mov r2 | */
    /* if comes from data, assume that token isn't empty,
    therefore it's relevant only for operators */
    raise_error (too_few_arguments_err, line->parts);
  }

  return ERROR_STATE;
}

state
target_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state)
{
  return operand_handler (line, symbol_table, &line->info.op->target,
                          next_state);
}

state
imm_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state)
{
  char *token = line->parts->token;
  int tmp;

  if (IS_EMPTY(token)) {
    if (line->info.data.len == 0) { /* .data _ */
      raise_error (empty_data_declaration_err, line->parts);
    }
    else { /* .data 1,_ */
      raise_error (unexpected_comma_after_end_err, line->parts);
    }
    return ERROR_STATE;
  }

  if (IS_COMMA(*token)) { /* .data ,1,2 | .data 1,2,,3 */
    raise_error (expected_integer_before_comma_err, line->parts);
    return ERROR_STATE;
  }

  if (!is_imm (line, token, symbol_table, &tmp)) {
    return ERROR_STATE;
  }

  line->info.data.arr[line->info.data.len++] = tmp;
  return (IS_EMPTY(line->parts->postfix) ? END_STATE : COMA_STATE);
}

state
str_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state)
{
  /* get the whole string into token */
  strcat (line->parts->token, line->parts->postfix);
  NULL_TERMINATE(line->parts->postfix, 0);

  if (!is_str (line->parts)) {
    return ERROR_STATE;
  }

  /*coping string without "" */
  strcpy (line->info.str.content, line->parts->token + 1);
  REMOVE_LAST_CHAR(line->info.str.content);
  line->info.str.len = (int) strlen (line->info.str.content);

  return next_state;
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

Bool valid_define_label (LineInfo *line, Symbol *symbol)
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

state
identifier_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state)
{
  char *token = line->parts->token, *target;
  Symbol *symbol;

  if (IS_EMPTY(token)) { /* .define _ | .extern _ | .entry _ */
    raise_error (expected_identifier_err, line->parts);
    return ERROR_STATE;
  }
  if (!is_valid_identifier (line->parts, token, TRUE)) {
    /* (not saved word && not abc123 && or not new name) */
    return ERROR_STATE;
  }

  symbol = find_symbol (symbol_table, line->parts->token);
  if (!symbol) {
    target = (line->type_l == def_l ?
              line->info.define.name : line->info.ext_ent.name);
    strcpy (target, token);
    return next_state;
  }
  else { /* symbol already exist, make sure it's valid */
    if ((line->type_l == ext_l && !valid_extern_label (line, symbol))
        || (line->type_l == ent_l && !valid_entry_label (line, symbol))
        || (line->type_l == def_l && !valid_define_label (line, symbol))) {
      return ERROR_STATE;
    }
  }
  return next_state;
}

state
equal_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state)
{
  if (IS_EMPTY(line->parts->token)) { /*.define x*/
    raise_error (empty_define_declaration_err, line->parts);
    return ERROR_STATE;
  }
  if (strcmp (line->parts->token, "=") != 0) { /*.define x y | .define x 3 */
    raise_error (expected_equals_before_expression_err, line->parts);
    return ERROR_STATE;
  }
  return next_state;
}

state
int_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state)
{
  char *token = line->parts->token;
  int *val = &line->info.define.val;

  if (IS_EMPTY(token)) { /* .define x = _ */
    raise_error (expected_numeric_expression_after_equal_err, line->parts);
    return ERROR_STATE;
  }
  if (is_int (token, val)) {
    if (*val >= MIN_INT_MACHINE && *val <= MAX_INT_MACHINE) {
      return next_state;
    }
    raise_error (exceeds_integer_bounds, line->parts);
    return ERROR_STATE;
  }
  /* else, it's invalid. exp: .define x = y | .define x = @#$#@ */
  raise_error (invalid_numeric_expression_err, line->parts);
  return ERROR_STATE;
}

state
extra_text_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state)
{
  char *token = line->parts->token;
  if (!IS_EMPTY(token)) {
    raise_error (extraneous_text_err, line->parts);
    return ERROR_STATE;
  }
  return next_state; /*end state*/
}

/* Gets the index of the specified state in the transition map array. */
int get_state_idx (transition map[], state state)
{
  int idx;
  for (idx = 0;; idx++) {
    if (state == map[idx].from)
      return idx;
  }
}

Bool run_fsm (LineInfo *line, Symbol_Table *symbol_table)
{
  transition *map = get_map (line);
  state next_state = map[0].from, next;
  int state_idx;

  while (next_state != END_STATE) {
    lineTok (line->parts);
    state_idx = get_state_idx (map, next_state);
    next = map[state_idx].next;
    next_state = map[state_idx].handler (line, symbol_table, next);
    if (next_state == ERROR_STATE) {
      return FALSE;
    }
  }
  return TRUE;
}