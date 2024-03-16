/*
 Created by OMER on 3/6/2024.
*/


#include "ast.h"


/***************** directive_lines *******************/

void init_op_analyze (op_analyze *op, Opcode opcode, LinePart *line)
{
  op->line_part = line;
  op->opcode = opcode;

  op->src.type = SRC;
  op->src.add_mode = NONE_ADD;
  op->src.param_types = param_types[opcode][1];

  op->target.type = TARGET;
  op->target.add_mode = NONE_ADD;
  op->target.param_types = param_types[opcode][2];
}

vector *init_op_list(void){
  return create_vector (sizeof (op_analyze));
}

op_analyze *add_to_op_list(vector* op_list, op_analyze *op){
  LinePart *tmp = malloc (sizeof (LinePart));
  if (!tmp){
    return NULL;
  }
  copy_line_info(tmp, op->line_part);
  op->line_part = tmp;
  return push (op_list, op);
}

size_t calc_op_size(op_analyze *op){
  size_t res = 1; /*for the first word */
  Addressing_Mode mode;

  /*special case of 2 registers that share the same word */
  if (op->src.add_mode == REG_ADD && op->target.add_mode == REG_ADD){
    res+=1;
  }
  else{
    /*word for src operand */
    mode = op->src.add_mode;
    if (mode != NONE_ADD){
      res += (mode == INDEX_ADD ? 2 : 1);
    }
    /*word for target operand */
    mode = op->target.add_mode;
    if (mode != NONE_ADD){
      res += (mode == INDEX_ADD ? 2 : 1);
    }
  }
  return res;
}

void print_operand(Operand *operand){
  switch (operand->add_mode) {
    case IMM_ADD:
      printf ("<imm: %d>\t", operand->val);
      break;
    case DIRECT_ADD:
      printf ("<symbol: %s>\t", operand->symbol_name);
      break;
    case INDEX_ADD:
      printf ("<index: %s[%d]>\t", operand->symbol_name, operand->val);
      break;
    case REG_ADD:
      printf ("<reg: r%d>\t", operand->val);
      break;
    case NONE_ADD:
      printf ("           \t");
      break;
  }
}

void print_op_analyze (op_analyze *op, char* file_name)
{
  if (op->errors == TRUE) {
    return;
  }
  printf ("%04lu\t", op->address);
  printf ("%s:%-2lu ", file_name, op->line_part->num);
  printf ("<op: %s>\t", op_names[op->opcode]);
  printf ("<opcode: %d>\t", op->opcode);
  print_operand(&(op->src));
  print_operand(&(op->target));
  printf ("\n");
}

void print_op_list(vector *op_list, char* file_name){
  size_t i;
  op_analyze *op;
  printf ("\n----------------- ast list ---------------------\n");
  for (i=0; i<op_list->size; ++i){
    op = (op_analyze*) get (op_list, i);
    print_op_analyze (op, file_name);
  }
}

void free_op_list(vector *op_list){
  size_t i;
  op_analyze *op;
  for (i=0; i<op_list->size; ++i){
    op = (op_analyze*) get(op_list,i);
    free(op->line_part);
  }
  free_vector (op_list);
}