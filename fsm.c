/*
 Created by OMER on 1/23/2024.
*/


#include "fsm.h"

#define MAX_IMM (2^11 -1)
#define MIN_IMM (-(2^11 -1))

#define CHAR_TO_INT(c) ((c) - '0')
#define IS_COMMA(s) ((s) == ',')
#define IMM_SIGN(s) ((s) == '#')
#define OPEN_INDEX_SIGN(s) (s[strlen(s)-1] == '[')
#define CLOSE_INDEX_SIGN(s) (s[strlen(s)-1] == ']')
#define IN_REG_BOUND(s) ((s) >= '0' && (s) <= '7')

/* 2 operators map (mov, cmp, add, sub, lea) */
transition two_operand_map[] = {
    {SRC_STATE,        &src_handler,        COMA_STATE},
    {COMA_STATE,       &comma_handler,      TARGET_STATE},
    {TARGET_STATE,     &target_handler,     EXTRA_TEXT_STATE},
    {EXTRA_TEXT_STATE, &extra_text_handler, END_STATE},
    {END_STATE, NULL,                       -1}
};

/* 1 operators map (not, clr, inc, dec, jmp, bne, red, prn, jsr)*/
transition one_operand_map[] = {
    {TARGET_STATE,     &target_handler,     EXTRA_TEXT_STATE},
    {EXTRA_TEXT_STATE, &extra_text_handler, END_STATE},
    {END_STATE, NULL,                       -1}
};

/* no operators map (rts, hlt)*/
transition zero_operand_map[] = {
    {EXTRA_TEXT_STATE, &extra_text_handler, END_STATE},
    {END_STATE, NULL,                       -1}
};

transition *get_map (Opcode o)
{
  if (o == MOV || o == CMP || o == ADD || o == SUB || o == LEA) {
    return two_operand_map;
  }
  if (o == NOT || o == CLR || o == INC || o == DEC || o == JMP || o == BNE ||
      o == RED || o == PRN || o == JSR) {
    return one_operand_map;
  }
  if (o == RTS || o == HLT) {
    return zero_operand_map;
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

Bool is_int2 (op_analyze *op, char *token, int *imm)
{
  char *end_ptr = NULL;
  /*todo check about long*/
  *imm = (int) strtol (token, &end_ptr, 10);
  return IS_EMPTY(end_ptr);
}

Bool is_define (op_analyze *op, char *token, LinkedList *symbol_table,
                int* imm)
{
  Node *node;
  Symbol *symbol;

  node = findNode (symbol_table, token);
  if (node) {
    symbol = (Symbol *) node->data;
    if (symbol->type == DEFINE) {
      *imm = symbol->val;
      return TRUE;
    }

    else { /* mov r0 STR | mov L[STR], r0 */
      /* todo maybe add the # to the prefix. print: #label*/
      r_error ("expected numeric value or defined constant, but got label ",
               op->line_part, "");
      return FALSE;
    }
  }
  /* mov r0, #zzz | mov L[zzz], r0 (when 'zzz' in undefined) */
  if (valid_identifier (op->line_part, token, FALSE)) {
    r_error ("", op->line_part, " undeclared");
    return FALSE;
  }
  /* mov r0 #3!a |  mov r0 L[***] | mov r0 L[] | mov #, r0 */
  r_error ("expected numeric value or defined constant, but got: ",
           op->line_part, "");
  return FALSE;
}

Bool is_imm2 (op_analyze *op, char *token, LinkedList *symbol_table, int *imm)
{
  /*  mov r0, #_ | mov r0, label[_ */
  if (IS_EMPTY(token)) {
    r_error ("expected numeric value or defined constant after ",
             op->line_part, "");
    return FALSE;
  }
  return is_int2 (op, token ,imm) || is_define (op, token, symbol_table, imm);
}


/* wanted 333] */
Bool is_index (op_analyze *op, LinkedList *symbol_table, int *val)
{
  char *token = op->line_part->token;
  if (!is_imm2 (op, token, symbol_table, val)) { /* mov r0 LABEL[&*$] */
    return FALSE;
  }

  /* check the ] sign */
  lineTok (op->line_part);
  if (IS_EMPTY(token)) { /* mov r0, L[1 */
    r_error ("expected ']'", op->line_part, "");
    return FALSE;
  }
  if (strcmp (token, "]") != 0) { /* mov r0, L[1 k] */
    r_error ("expected ']' before", op->line_part, "");
    return FALSE;
  }
  return TRUE;
}

/*todo outside if empty*/
addr_mode_flag get_addressing_mode (op_analyze *op, LinkedList *symbol_table,
                                    Operand *operand)
{
  char *token = op->line_part->token;
  char tmp[MAX_LINE_LENGTH];
  /*register*/
  if (is_reg (token, &operand->val)) {
    return b_reg;
  }

  /*imm*/
  if (IMM_SIGN (token[0])) {
    strcpy (tmp, token + 1); /* remove # sign */
    return is_imm2 (op, tmp, symbol_table, &operand->val) ? b_imm : 0;
  }

  /* index */
  if (OPEN_INDEX_SIGN(token)) {
    strcpy (operand->symbol_name, token);
    NULL_TERMINATE(operand->symbol_name, strlen (token) - 1); /* remove [ char */
    if (valid_identifier (op->line_part, operand->symbol_name, FALSE)) {
      lineTok (op->line_part); /* move to imm */
      return is_index (op, symbol_table, &operand->val) ? b_index : 0;
    }
  }

  /*symbol*/
  if (valid_identifier (op->line_part, token, FALSE)) {
    strcpy (operand->symbol_name, token);
    return b_symbol;
  }

  r_error ("", op->line_part, " invalid operand");
  return 0;
}

Bool valid_add_mode (op_analyze *op, Operand *operand, addr_mode_flag add_mode)
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
  r_error ("passing ", op->line_part, " is invalid addressing mode to "
                                      "function");
  return FALSE;
}

state operand_handler (op_analyze *op, LinkedList *symbol_table,
                       Operand *operand, state next_state)
{
  addr_mode_flag add_mode;
  char *token = op->line_part->token;

  /* check for missing arg (exp: mov r2, _ | mov _) */
  if (IS_EMPTY(token)) {
    r_error ("too few arguments in instruction ", op->line_part, "");
    return ERROR_STATE;
  }

  /* check for redundant comma (exp: mov, r2, r3 | mov r2,,r3) */
  if (IS_COMMA(*token)) {
    r_error ("expected expression before ", op->line_part, " token");
    return ERROR_STATE;
  }

  /* find the addressing mode and assign it to the appropriate operand*/
  add_mode = get_addressing_mode (op, symbol_table, operand);

  /*if couldn't find any addressing mode */
  if (add_mode == 0) {
    return ERROR_STATE;
  }

  /* check if it is a valid addressing mode */
  if (!valid_add_mode (op, operand, add_mode)) {
    return ERROR_STATE;
  }
  return next_state;
}

state src_handler (op_analyze *op, file_analyze *file, state next_state)
{
  return operand_handler (op, file->symbol_table, &op->src, next_state);
}

state comma_handler (op_analyze *op, file_analyze *file, state next_state)
{
  /* unused param */
  char *token = op->line_part->token;
  if (IS_COMMA(*token)) {
    return next_state;
  }
  if (IS_EMPTY(token)) { /*mov r0 */
    r_error ("too few arguments in instruction", op->line_part, "");
  }
  else { /*mov r0 r2*/
    r_error ("missing comma before ", op->line_part, "");
  }
  file->error += ERROR; /*todo done only because it's unused... */
  return ERROR_STATE;
}

state target_handler (op_analyze *op, file_analyze *file, state next_state)
{
  return operand_handler (op, file->symbol_table, &op->target, next_state);
}

state extra_text_handler (op_analyze *op, file_analyze *file, state next_state)
{
  char *token = op->line_part->token;
  if (!IS_EMPTY(token)) {
    /*Concatenates the entire continuation of the line for the error msg */
    strcat (op->line_part->token, op->line_part->postfix);
    NULL_TERMINATE(op->line_part->postfix, 0);
    r_error ("unexpected text after end of the command: ", op->line_part, "");
    file->error += ERROR; /*todo done only veacuse it's unused... */
    return ERROR_STATE;
  }
  return next_state; /*end state*/
}

int run_fsm (op_analyze *op, file_analyze *file_analyze)
{
  transition *map = get_map (op->opcode);
  state next_state = map[0].from, next;
  int i = 0;
/*   int stateIdx; */

  while (next_state != END_STATE) {
    lineTok (op->line_part);
/*     stateIdx = get_state_idx (op_info->map, next_state); */
    next = map[i].next;
    next_state = map[i].handler (op, file_analyze, next);
    if (next_state == ERROR_STATE) {
      op->errors = TRUE;
      return FALSE;
    }
    ++i;
  }
  return TRUE;
}