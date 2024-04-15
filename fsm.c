#include "fsm.h"
/* =======================================================================
 *                              fsm definition
 * ======================================================================= */

typedef enum state
{
    SRC_STATE,
    COMA_STATE,
    TARGET_STATE,
    STRING_STATE,
    IMM_STATE,
    IDENTIFIER_STATE,
    EQUAL_STATE,
    INT_STATE,
    EXTRA_TEXT_STATE,
    END_STATE,
    ERROR_STATE
} state;

typedef state (*handler) (LineInfo *line, Symbol_Table *table, state nextState);

typedef struct transition
{
    state from;
    handler handler;
    state next;
} transition;

/* ----------------------------- handlers ----------------------------- */
state str_handler       (LineInfo *line, Symbol_Table *symbol_table, state next_state);
state imm_handler       (LineInfo *line, Symbol_Table *symbol_table, state next_state);
state identifier_handler(LineInfo *line, Symbol_Table *symbol_table, state next_state);
state equal_handler     (LineInfo *line, Symbol_Table *symbol_table, state next_state);
state int_handler       (LineInfo *line, Symbol_Table *symbol_table, state next_state);
state src_handler       (LineInfo *line, Symbol_Table *symbol_table, state next_state);
state comma_handler     (LineInfo *line, Symbol_Table *symbol_table, state next_state);
state target_handler    (LineInfo *line, Symbol_Table *symbol_table, state next_state);
state extra_text_handler(LineInfo *line, Symbol_Table *symbol_table, state next_state);

/* helpers function */
transition*     get_map (LineInfo *line);
int             get_state_idx (transition map[], state state);
addr_mode_flag  get_addressing_mode (LineInfo *line, Symbol_Table *table, Operand *operand);
state           operand_handler (LineInfo *line, Symbol_Table *table, Operand *operand, state next_state);

/* --------------------------- transition map --------------------------- */

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

#define PSEUDO_USE_OF_SYMBOL_TABLE(s) (s->unresolved_symbols_count+=0)


/* =======================================================================
 *                              fsm implementation
 * ======================================================================= */

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

/**
 * Retrieves the transition map based on the line type.
 */
transition *get_map (LineInfo *line)
{
  Opcode o;
  switch (line->type) {
    case str_line:
      return str_map;
    case data_line:
      return data_map;
    case def_line:
      return define_map;
    case ext_line:
    case ent_line:
      return ext_ent_map;
    case op_line:
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

/**
 * Gets the index of the specified state in the transition map array.
 */
int get_state_idx (transition map[], state state)
{
  int idx;
  for (idx = 0;; idx++) {
    if (state == map[idx].from)
      return idx;
  }
}

/**
 * Handles source operand state.
 */
state
src_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state)
{
  return operand_handler (line, symbol_table, &line->info.op->src,
                          next_state);
}

/**
 * Handles target operand state.
 */
state
target_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state)
{
  return operand_handler (line, symbol_table, &line->info.op->target,
                          next_state);
}

/**
 * Handles operand state (both src and target): find its address mode and
 * check it's valid.
 */
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

/**
 * analyze the given operand and find its addressing mode
 */
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

/**
 * Handles comma operand state - comma should come after operand or data
 */
state
comma_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state)
{

  char *token = line->parts->token;
  if (IS_COMMA(*token)) {
    return next_state;
  }

  if (!IS_EMPTY(token)) { /* mov r0 r3 | .data 1 2 */
    raise_error (expected_comma_before_expression_err, line->parts);
  }
  else { /* mov r2 | */
    /* if comes from data, assume that token isn't empty,
    therefore it's relevant only for operators */
    raise_error (too_few_arguments_err, line->parts);
  }

  PSEUDO_USE_OF_SYMBOL_TABLE(symbol_table);
  return ERROR_STATE;
}

/**
 * Handles immediate state.
 * state used for both int or constant val (define)
 * */
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
  /* next state is comma state */
  return (IS_EMPTY(line->parts->postfix) ? END_STATE : next_state);
}

/**
 * Handles string state.
 */
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

  PSEUDO_USE_OF_SYMBOL_TABLE(symbol_table);
  return next_state;
}

/**
 * Handles identifier state - label of extern, entry or define
 */
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
    target = (line->type == def_line ?
              line->info.define.name : line->info.ext_ent.name);
    strcpy (target, token);
    return next_state;
  }
  else { /* symbol already exist, make sure it's valid declaration */
    if ((line->type == ext_line && !valid_extern_label (line, symbol))
        || (line->type == ent_line && !valid_entry_label (line, symbol))
        || (line->type == def_line && !valid_define_symbol (line, symbol))) {
      return ERROR_STATE;
    }
  }
  return next_state;
}

/**
 * Handles equal state - that come after a define
 */
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
  PSEUDO_USE_OF_SYMBOL_TABLE(symbol_table);
  return next_state;
}

/**
 * Handles integer state - integer must appear after a define
 */
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
  PSEUDO_USE_OF_SYMBOL_TABLE(symbol_table);
  return ERROR_STATE;
}

/**
 * Handles extra text state - make sure at the end of every line that there
 * is no extraneous text
 *
 * @errors - extraneous_text_err
 */
state
extra_text_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state)
{
  char *token = line->parts->token;
  if (!IS_EMPTY(token)) {
    raise_error (extraneous_text_err, line->parts);
    return ERROR_STATE;
  }
  PSEUDO_USE_OF_SYMBOL_TABLE(symbol_table);
  return next_state; /*end state*/
}
