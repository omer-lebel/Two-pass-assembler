/*
 Created by OMER on 1/23/2024.
*/


#include "fsm.h"

#define MAX_IMM (2^11 -1)
#define MIN_IMM (-(2^11 -1))

#define CHAR_TO_INT(c) ((c) - '0')
#define IS_COMMA(s) ((s) == ',')
#define IMM_SIGN(s) ((s) == '#')
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
  //todo check about &
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
/*
 * {two_operand_map: mov, cmp, add, sub, lea}
 * {on_operand_map: not, clr, inc, dec, jmp, bne, red, prn, jsr}
 * {no operators map rts, hlt }
 * {entry_extern_map: entry, extern}
 * {str}
 * {data}
 */

Bool is_reg (const char *str, int *val)
{
  if (str[0] == 'r' && IN_REG_BOUND(str[1]) && str[2] == '\0') {
    *val = CHAR_TO_INT(str[1]);
    return TRUE;
  }
  *val = INVALID_REG;
  return FALSE;
}

Bool is_int2 (op_analyze *op, int *imm)
{
  char *end_ptr = NULL;
  /*todo check about long*/
  *imm = (int) strtol (op->line_info->token, &end_ptr, 10);
  return IS_EMPTY(end_ptr);
}

Bool is_define (op_analyze *op, LinkedList *symbol_table, int *imm)
{
  Node *node;
  Symbol *symbol;
  char *token = op->line_info->token;

  node = findNode (symbol_table, token);
  if (node) {
    symbol = (Symbol *) node->data;
    if (symbol->type == DEFINE) {
      *imm = symbol->val;
      return TRUE;
    }

    else { /* mov r0 STR | mov L[STR], r0 */
      r_error ("expected numeric value or defined constant, but got label ",
               op->line_info, "");
      return FALSE;
    }
  }
  /* mov r0, #zzz | mov L[zzz], r0 (when 'zzz' in undefined) */
  if (valid_identifier (op->line_info, token, FALSE)) {
    r_error ("", op->line_info, " undeclared");
    return FALSE;
  }
  /* mov r0 #3!a |  mov r0 L[***] | mov r0 L[] | mov #, r0 */
  r_error ("expected numeric value or defined constant, but got: ",
           op->line_info, "");
  return FALSE;
}

Bool is_imm2 (op_analyze *op, LinkedList *symbol_table, int *imm)
{
  // mov r0, #_ | mov r0, label[_ */
  if (IS_EMPTY(op->line_info->token)) {
    r_error ("expected numeric value or defined constant",
             op->line_info, "");
    return FALSE;
  }
  return is_int2 (op, imm) || is_define (op, symbol_table, imm);
}

Bool is_data_symbol (op_analyze *op, LinkedList *symbol_table, Node **node)
{
  Symbol *symbol;
  *node = findNode (symbol_table, op->line_info->token);
  if (*node) {
    symbol = (Symbol *) (*node)->data;
    // todo add test, maybe there is no need for that!?
    if (symbol->type == DATA) { /* .string  | .data | .extern */
      return TRUE;
    }
    else {
      // LABEL: mov r1, r2
      // move #2, LABEL
      r_warning ("expected directive label, but label", op->line_info,
                 "is of invalid type");
    }
  }
  return FALSE;
}

Bool index_indicate (op_analyze *op)
{
  char *str = op->line_info->postfix;

  // check that next token start with [
  while (*str && isspace(*str) && *str != '[') {
    str++;
  }
  if (*str != '[') {
    return FALSE;
  }
  return TRUE;
}

Bool is_index (op_analyze *op, LinkedList *symbol_table, int *val)
{
  char *token = op->line_info->token;
  if (!is_imm2 (op, symbol_table, val)) { /* mov r0 LABEL[&*$] */
    return FALSE;
  }

  //check the ] sign
  lineTok (op->line_info);
  if (IS_EMPTY(token)) { /* mov r0, L[1 */
    r_error ("expected ']'", op->line_info, "");
    return FALSE;
  }
  if (strcmp (token, "]") != 0) { /* mov r0, L[1 k] */
    r_error ("expected ']' before", op->line_info, "");
    return FALSE;
  }
  return TRUE;
}

/*todo outside if empty*/
Addressing_Mode get_addressing_mode (op_analyze *op, LinkedList *symbol_table,
                                     state curr_state)
{
  char *token = op->line_info->token;
  int *val = (curr_state == SRC_STATE) ? &(op->src.val) : &(op->target.val);
  char *symbol_name = (curr_state == SRC_STATE) ? (op->src.symbol_name) :
      (op->target.symbol_name);

  /*register*/
  if (is_reg (token, val)) {
    return REG_ADD;
  }

  /*imm*/
  if (IMM_SIGN (*token)) {
    lineTok (op->line_info);
    return is_imm2 (op, symbol_table, val) ? IMM_ADD : NONE_ADD;
  }

  /*symbol and index*/
  if (valid_identifier(op->line_info, token, FALSE)) {
    strcpy (symbol_name, token);

    /*label*/
    if (!index_indicate (op)) {
      return DIRECT_ADD;
    }

    /*index*/
    else { //fount [
      lineTok (op->line_info); //move to [
      lineTok (op->line_info); //move to imm
      return is_index (op, symbol_table, val) ? INDEX_ADD : NONE_ADD;
    }
  }

  r_error ("", op->line_info, " invalid operand");
  return NONE_ADD;
}

Bool valid_add_mode (op_analyze *op, state operand)
{
  if (operand == SRC_STATE) {
    if (op->propriety->src_modes[op->src.add_mode]) {
      return TRUE;
    }
  }
  else { /*operand == TARGET_STATE*/
    if (op->propriety->target_modes[op->target.add_mode]) {
      return TRUE;
    }
  }
  r_error ("passing ", op->line_info, " is invalid addressing mode to "
                                        "function");
  return FALSE;
  }

state operand_handler (op_analyze *op, LinkedList *symbol_table, state
curr_state, state next_state)
{
  Addressing_Mode add_mode;
  char *token = op->line_info->token;

  /* check for missing arg (exp: mov r2, _ | mov _) */
  if (IS_EMPTY(token)) {
    r_error ("too few arguments in instruction ", op->line_info, "");
    return ERROR_STATE;
  }

  /* check for redundant comma (exp: mov, r2, r3 | mov r2,,r3) */
  if (IS_COMMA(*token)) {
    r_error ("expected expression before ", op->line_info, " token");
    return ERROR_STATE;
  }

  /* find the addressing mode and assign it to the appropriate operand*/
  add_mode = get_addressing_mode (op, symbol_table, curr_state);
  if (curr_state == SRC_STATE) {
    op->src.add_mode = add_mode;
  }
  else { //target
    op->target.add_mode = add_mode;
  }

  /*if couldn't find any addressing mode */
  if (add_mode == NONE_ADD) {
    return ERROR_STATE;
  }

  /* check if it is a valid addressing mode */
  if (!valid_add_mode (op, curr_state)) {
    return ERROR_STATE;
  }
  return next_state;
}

state src_handler (op_analyze *op, file_analyze *file, state next_state)
{
  return operand_handler (op, file->symbol_table, SRC_STATE, next_state);
}

state comma_handler (op_analyze *op, file_analyze *file, state next_state)
{
  char *token = op->line_info->token;
  if (IS_COMMA(*token)) {
    return next_state;
  }
  if (IS_EMPTY(token)) { /*mov r0 */
    r_error ("too few arguments in instruction", op->line_info, "");
  }
  else { /*mov r0 r2*/
    r_error ("missing comma before ", op->line_info, "");
  }
  return ERROR_STATE;
}

state target_handler (op_analyze *op, file_analyze *file, state next_state)
{
  return operand_handler (op, file->symbol_table, TARGET_STATE, next_state);
}

state extra_text_handler (op_analyze *op, file_analyze *file, state next_state)
{
  char *token = op->line_info->token;
  if (!IS_EMPTY(token)) {
    /*Concatenates the entire continuation of the line for the error msg */
    strcat (op->line_info->token, op->line_info->postfix);
    NULL_TERMINATE(op->line_info->postfix, 0);
    r_error ("unexpected text after end of the command: ", op->line_info, "");
    return ERROR_STATE;
  }
  return next_state; /*end state*/
}

int run_fsm (op_analyze *op, file_analyze *file_analyze)
{
  transition *map = get_map (op->propriety->opcode);
  state next_state = map[0].from, next;
  int i = 0;
//  int stateIdx;

  while (next_state != END_STATE) {
    lineTok (op->line_info);
//    stateIdx = get_state_idx (op_info->map, next_state);
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