//
// Created by OMER on 1/23/2024.
//

#include "fsm.h"

#define MAX_IMM (2^11 -1)
#define MIN_IMM -(2^11 -1)

#define IS_COMMA(str) (strcmp(str,",") == 0)
#define CHAR_TO_INT(c) ((c) - '0')

typedef enum mode {SCR, TARGET} mode;

/* mov */
transition map_mov[] = {
    {SRC_STATE,        &src_handler,        COMA_STATE},
    {COMA_STATE,       &comma_handler,      TARGET_STATE},
    {TARGET_STATE,     &target_handler,     EXTRA_TEXT_STATE},
    {EXTRA_TEXT_STATE, &extra_text_handler, END_STATE},
    {END_STATE, NULL,                       -1}
};

Bool is_reg (char *str, int *val)
{
  if (str[0] == 'r' && str[1] >= '0' && str[1] <= '7' && str[2] == '\0') {
    *val = CHAR_TO_INT(str[1]);
    return TRUE;
  }
  *val = NO_REG;
  return FALSE;
}

Bool is_int2 (char *str, int *imm)
{
  char *end_ptr = NULL;
  *imm = strtol (str, &end_ptr, 10);
  if (!IS_EMPTY(end_ptr)) {
    return FALSE;
  }
  return TRUE;
}

Bool is_define (char *str, int *imm)
{
  Node *node;
  Symbol *symbol;

  node = findNode (symbols_table, str);
  if (node) {
    symbol = (Symbol *) node->data;
    if (symbol->type == DEFINE) {
      *imm = symbol->val;
      return TRUE;
    }
    else {
      /*raise error about label type*/
    }
  }
  return FALSE;
}

Bool is_imm2 (char *str, int *imm)
{
  return is_int2 (str, imm) || is_define (str, imm);
}

Bool is_data_symbol (char *str, Node **node)
{
  Symbol *symbol;

  *node = findNode (symbols_table, str);
  if (*node) {
    symbol = (Symbol *) (*node)->data;
    if (symbol->type == DIRECTIVE) { /* .string  | .data | .extern */
      return TRUE;
    }
    else {
      /*raise error about label type*/
    }
  }
  return FALSE;
}

/*check that it's a valid name for symbol*/
Bool maybe_symbol(char *str){
  return (isalpha(str[0]) && isAlphaNumeric (str) && !isSavedWord (str));
}

/*

Bool is_index_pattern(char *str, char *symbol_name, char imm_name){

  char *open_bracket = strchr(str, '[');
  char *close_bracket = str + strlen (str) - 1;
  if (open_bracket == NULL){ */
/*doesn't have opening [ *//*

    return FALSE;
  }

  if (*close_bracket != ']'){ */
/*doesn't end with ] *//*

    return FALSE;
  }

  if (close_bracket - open_bracket < 1){
    return FALSE;
  }

  return TRUE;
}
*/

/*todo outside if empty*/
int get_addressing_mode (char *token, Node **symbol_node, int *val)
{
  /*imm*/
  if (token[0] == '#' && is_imm2 (token + 1, val)) {
    return IMM_ADD;
  }

  /*register*/
  if (is_reg (token, val)) {
    return REG_ADD;
  }

  /*symbol*/ //todo think about where the error msg
  if (is_data_symbol (token, symbol_node) || maybe_symbol (token)) {
    return DIRECT_ADD;
  }
  /*index*/

  return NONE_ADD;
}

Bool valid_add_mode (op_analyze *op, state operand)
{
  if (operand == SRC_STATE && op->propriety->src_modes[op->src.add_mode]) {
    return TRUE;
  }
  else if (op->propriety->target_modes[op->target.add_mode]) { /*target*/
    return TRUE;
  }

  else {
    r_error ("invalid add mode ", op->line_info, "");
    return FALSE;
  }
}

state src_handler (op_analyze *op, state nextState)
{
  Addressing_Mode add_mode;
  char *token = op->line_info->token;

  if (IS_EMPTY(token)) {
    r_error ("no src", op->line_info, "");
    return ERROR_STATE;
  }
  if (IS_COMMA(token)){
    r_error ("expected expression before ", op->line_info, " token");
    return ERROR_STATE;
  }

  add_mode = get_addressing_mode (token, &(op->src.symbol), &(op->src.val));
  op->src.add_mode = add_mode;
  if (!valid_add_mode (op, SRC_STATE)) { /*true --> scr operand */
    r_error ("invalid address mode", op->line_info, "");
  }
  return nextState;
}

state comma_handler (op_analyze *op, state nextState){
  char *token = op->line_info->token;
  if (IS_COMMA(token)){
    return nextState;
  }

  op->errors = TRUE;
  if (IS_EMPTY(token)) { /*mov r0 */
    r_error ("missing arg", op->line_info, "");
  }
  else{ /*mov r0 r2*/
    r_error ("missing comma", op->line_info, "");
  }
  return ERROR_STATE;
}

state target_handler (op_analyze *op, state nextState){
  Addressing_Mode add_mode;
  char *token = op->line_info->token;

  if (IS_EMPTY(token)) {
    r_error ("no target", op->line_info, "");
    return ERROR_STATE;
  }
  /*in case of op that get 1 operand, */
  if (IS_COMMA(token)){
    r_error ("expected expression before ", op->line_info, " token");
    return ERROR_STATE;
  }

  add_mode = get_addressing_mode (token, &(op->target.symbol), &(op->target.val));
  op->target.add_mode = add_mode;
  if (!valid_add_mode (op, TARGET_STATE)) {
    return ERROR_STATE;
  }
  return nextState;
}

state extra_text_handler (op_analyze *op, state nextState){
  char *token = op->line_info->token;
  if (!IS_EMPTY(token)){
    /*Concatenates the entire continuation of the line for the error msg */
    strcat (op->line_info->token, op->line_info->postfix);
    NULL_TERMINATE(op->line_info->postfix, 0);
    r_error ("unexpected text after end of the command: ", op->line_info, "");
    return ERROR_STATE;
  }
  return nextState; /*end state*/
}



int run_fsm (op_analyze *op)
{
  state nextState = map_mov[0].from, next;
  int i = 0;
//  int stateIdx;

  while (nextState != END_STATE) {
    lineTok (op->line_info);
//    stateIdx = get_state_idx (op_info->map, nextState);
    next = map_mov[i].next;
    nextState = map_mov[i].handler (op,next);
    if (nextState == ERROR_STATE) {
      return FALSE;
    }
    ++i;
  }
  return TRUE;
}










//Bool is_define_imm(char* str, int* val);
//
//Bool is_imm(char* str, int* val);
//
//Bool is_index(char* str, int* val);
//
//Bool is_label(char* str, int* val);
//
//exit_code get_src_add_mode();
//
//exit_code get_target_add_mode();




//state src_handler(LineInfo *line);
//
//state target_handler(LineInfo *line);
//
//state comma_handler(LineInfo *line);
//
//state extra_text_handler();
//
//exit_code run_fsm();