/*
 Created by OMER on 1/23/2024.
*/


#include "fsm.h"

#define MAX_IMM (2^11 -1)
#define MIN_IMM (-(2^11 -1))

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

transition data_map[] = {
    {IMM_STATE,        &imm_handler,        COMA_STATE},
    {COMA_STATE,       &comma_handler,      IMM_STATE},
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
  switch (line->type_t) {
    case str_l:
      return str_map;
    case def_l:
      return define_map;
    case ext_l:
      return ext_ent_map;
    case ent_l:
      return ext_ent_map;
    case op_l:
      o = line->op->opcode;
      if (o == MOV || o == CMP || o == ADD || o == SUB || o == LEA) {
        return two_operand_map;
      }
      if (o == NOT || o == CLR || o == INC || o == DEC || o == JMP || o == BNE
          ||
          o == RED || o == PRN || o == JSR) {
        return one_operand_map;
      }
      if (o == RTS || o == HLT) {
        return zero_operand_map;
      }
    case data_l:
      return data_map;
  }
  return NULL;
}

Bool is_reg (const char *str, int *val)
{
  if (str[0] == 'r' && IN_REG_BOUND(str[1]) && str[2] == '\0') {
    *val = CHAR_TO_INT(str[1]);
    return TRUE;
  }
  return FALSE;
}

Bool is_int (char *token, int *imm)
{
  char *end_ptr = NULL;
  /*todo check about long*/
  *imm = (int) strtol (token, &end_ptr, 10);
  return IS_EMPTY(end_ptr);
}

Bool is_define (LineInfo *line, char *token, LinkedList *symbol_table,
                int *imm)
{
  Symbol *symbol;
  Node *node = findNode (symbol_table, token);
  if (node) {
    symbol = (Symbol *) node->data;
    if (symbol->type == DEFINE) {
      *imm = symbol->val;
      return TRUE;
    }
    else { /* mov r0 #STR | mov r0 LABEL[STR] */
      /* todo maybe add the # to the prefix. print: #label*/
      r_error ("expected numeric value or defined constant, but got label ",
               line->parts, "");
      return FALSE;
    }
  }
  /* mov r0, #zzz | mov L[zzz], r0 | .data 1,zzz (when 'zzz' in undefined) */
  if (valid_identifier (line->parts, token, FALSE)) {
    r_error ("", line->parts, " undeclared");
    return FALSE;
  }
  /* mov r0 #3!a |  mov r0 L[***] | mov r0 L[] | mov #, r0 | .data 1,2, $ */
  r_error ("expected numeric value or defined constant, but got: ",
           line->parts, "");
  return FALSE;
}

Bool is_imm (LineInfo *line, char *token, LinkedList *symbol_table, int *imm)
{
  if (IS_EMPTY(token) && line->type_t != data_l) {
    /*  mov r0, #_  | mov r0, L [_] */
    /* if comes from data, assume that token isn't empty,
    therefore it's relevant only for operators */
    r_error ("expected numeric value or defined constant after ",
             line->parts, "");
    return FALSE;
  }
  return is_int (token, imm) || is_define (line, token, symbol_table, imm);
}

/* wanted 333] */
Bool is_index (LineInfo *line, LinkedList *symbol_table, int *val)
{
  char *token = line->parts->token;
  if (!is_imm (line, token, symbol_table, val)) { /* mov r0 LABEL[&*$] */
    return FALSE;
  }

  /* check the ] sign */
  lineTok (line->parts);
  if (IS_EMPTY(token)) { /* mov r0, L[1 */
    r_error ("expected ']'", line->parts, "");
    return FALSE;
  }
  if (!IS_CLOSE_INDEX_SIGN(token)) { /* mov r0, L[1 k] */
    r_error ("expected ']' before", line->parts, "");
    return FALSE;
  }
  return TRUE;
}

Bool is_str (LinePart *line)
{
  char *str = line->token;
  size_t len = strlen (line->token);

  if (IS_EMPTY (str)) { /* .string _ */
    r_error ("empty string declaration", line, "");
    return FALSE;
  }
  if (str[0] != '"') { /* .string a" */
    r_error ("missing opening \" character in ", line, "");
    return FALSE;
  }
  if (len == 1 || str[len - 1] != '"') { /*.string " || .string "abc   */
    r_error ("missing terminating \" character in ", line, "");
    return FALSE;
  }
  return TRUE;
}

addr_mode_flag get_addressing_mode (LineInfo *line, LinkedList *symbol_table,
                                    Operand *operand)
{
  char *token = line->parts->token;
  char tmp[MAX_LINE_LENGTH];
  /*register*/
  if (is_reg (token, &operand->val)) {
    return b_reg;
  }

  /*imm*/
  if (IS_IMM_SIGN (token[0])) {
    strcpy (tmp, token + 1); /* remove # sign */
    return is_imm (line, tmp, symbol_table, &operand->val) ? b_imm : 0;
  }

  /* index */
  if (IS_OPEN_INDEX_SIGN(token[strlen (token) - 1])) {
    strcpy (operand->symbol_name, token);
    NULL_TERMINATE(operand->symbol_name,
                   strlen (token) - 1); /* remove [ char */
    if (valid_identifier (line->parts, operand->symbol_name, FALSE)) {
      lineTok (line->parts); /* move to imm */
      return is_index (line, symbol_table, &operand->val) ? b_index : 0;
    }
  }

  /*symbol*/
  if (valid_identifier (line->parts, token, FALSE)) {
    strcpy (operand->symbol_name, token);
    return b_symbol;
  }

  r_error ("", line->parts, " invalid operand");
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
  r_error ("passing ", line->parts, " is invalid addressing mode to "
                                    "function");
  return FALSE;
}

state operand_handler (LineInfo *line, LinkedList *symbol_table,
                       Operand *operand, state next_state)
{
  addr_mode_flag add_mode;
  char *token = line->parts->token;

  /* check for missing arg (exp: mov r2, _ | mov _) */
  if (IS_EMPTY(token)) {
    r_error ("too few arguments in instruction ", line->parts, "");
    return ERROR_STATE;
  }

  /* check for redundant comma (exp: mov, r2, r3 | mov r2,,r3) */
  if (IS_COMMA(*token)) {
    r_error ("expected expression before ", line->parts, " token");
    return ERROR_STATE;
  }

  /* find the addressing mode and assign it to the appropriate operand*/
  add_mode = get_addressing_mode (line, symbol_table, operand);

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

state src_handler (LineInfo *line, file_analyze *file, state next_state)
{
  return operand_handler (line, file->symbol_table, &line->op->src,
                          next_state);
}

//.data 1,2,3,
// mov r2 r3

state comma_handler (LineInfo *line, file_analyze *file, state next_state)
{
  char *token = line->parts->token;
  if (IS_COMMA(*token)) {
    return next_state;
  }

  if (!IS_EMPTY(token)) { /* mov r0 r3 | .data 1 2 */
    r_error ("expected ',' before ", line->parts, "");
  }
  else{ /* mov r2 | */
    /* if comes from data, assume that token isn't empty,
    therefore it's relevant only for operators */
    r_error ("too few arguments in instruction", line->parts, "");
  }

  return ERROR_STATE;
}

state target_handler (LineInfo *line, file_analyze *file, state next_state)
{
  return operand_handler (line, file->symbol_table, &line->op->target,
                          next_state);
}

state imm_handler (LineInfo *line, file_analyze *file, state next_state)
{
  char *token = line->parts->token;
  int tmp;

  if (IS_EMPTY(token)) {
    if (line->data.len == 0){ /* .data _ */
      r_error ("empty data initializer", line->parts, "");
    }
    else{ /* .data 1,_ */
      r_error ("unexpected ',' after end of command", line->parts, "");
    }
    return ERROR_STATE;
  }

  if (IS_COMMA(*token)){
    r_error ("expected integer before ", line->parts, "token");
    return ERROR_STATE;
  }

  if (!is_imm (line, token, file->symbol_table, &tmp)) {
    return ERROR_STATE;
  }

  line->data.arr[line->data.len++] = tmp;
  return (IS_EMPTY(line->parts->postfix) ? END_STATE : COMA_STATE);
}


state str_handler (LineInfo *line, file_analyze *file, state next_state)
{
  /* get the whole string into token */
  strcat (line->parts->token, line->parts->postfix);
  NULL_TERMINATE(line->parts->postfix, 0);

  if (!is_str (line->parts)) {
    file->error += ERROR; /*todo done only because it's unused... */
    return ERROR_STATE;
  }

  /*coping string without "" */
  strcpy (line->str.content, line->parts->token + 1);
  line->str.len = strlen (line->str.content) - 1;
  NULL_TERMINATE(line->str.content, line->str.len);

  return next_state;
}

state identifier_handler (LineInfo *line, file_analyze *file, state next_state)
{
  char *token = line->parts->token, *target;

  if (IS_EMPTY(token)) { /* .define _ */
    r_error ("expected identifier after directive", line->parts, "");
    return ERROR_STATE;
  }
  if (!valid_identifier (line->parts, token, TRUE)) {
    /* (not saved word && not abc123 && or not new name) */
    return ERROR_STATE;
  }
  /* todo maybe outside? there is an error entry */
  if (findNode (file->symbol_table, token)) {
    r_error ("redeclaration of ", line->parts, "");
    return ERROR_STATE;
  }

  target = (line->type_t == def_l ? line->define.name : line->ext_ent.name);
  strcpy (target, token);

  return next_state;
}

state equal_handler (LineInfo *line, file_analyze *file, state next_state)
{
  if (IS_EMPTY(line->parts->token)) {
    r_error ("empty define declaration", line->parts, "");
    return ERROR_STATE;
  }
  if (strcmp (line->parts->token, "=") != 0) { /*.define x y | .define x 3 */
    r_error ("expected '=' before numeric token, but got ", line->parts, "");
    return ERROR_STATE;
  }
  return next_state;
}

state int_handler (LineInfo *line, file_analyze *file, state next_state)
{
  char *token = line->parts->token;

  if (IS_EMPTY(token)) {
    r_error ("expected numeric expression after '='", line->parts, "");
    return ERROR_STATE;
  }
  if (!is_int (token, &line->define.val)) {
/*    if (res > MAX_INT || res < MIN_INT) {
      r_error ("", line, " exceeds integer bounds [-(2^13-1), 2^13-1]");
    }*/
    r_error ("", line->parts, " is not a valid numeric expression");
    return ERROR_STATE;
  }
  return next_state;
}

state extra_text_handler (LineInfo *line, file_analyze *file, state next_state)
{
  char *token = line->parts->token;
  if (!IS_EMPTY(token)) {
    /*Concatenates the entire continuation of the line for the error msg */
    strcat (line->parts->token, line->parts->postfix);
    NULL_TERMINATE(line->parts->postfix, 0);

    r_error ("unexpected text after end of command: ", line->parts, "");
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

Bool run_fsm (LineInfo *line, file_analyze *file_analyze)
{
  transition *map = get_map (line);
  state next_state = map[0].from, next;
  int state_idx;

  while (next_state != END_STATE) {
    lineTok (line->parts);
    state_idx = get_state_idx (map, next_state);
    next = map[state_idx].next;
    next_state = map[state_idx].handler (line, file_analyze, next);
    if (next_state == ERROR_STATE) {
      line->error_t = TRUE;
      return FALSE;
    }
  }
  return TRUE;
}