/*
 Created by OMER on 3/6/2024.
*/


#include "ast.h"

#define CYN   "\x1B[36m"
#define RESET "\x1B[0m"

/***************** directive_lines *******************/

void init_op_analyze (op_analyze *op, Opcode opcode)
{
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

void print_op_analyze (op_analyze *op)
{
  printf ("%04lu\t", op->address);
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
    print_op_analyze (op);
  }
}

void free_op_list(vector *op_list){
  free_vector (op_list);
}

void print_data(int *arr, unsigned len){
  unsigned int i;
  printf (CYN "data: " RESET "%d", arr[0]);
  for (i = 1; i < len ; ++i) {
    printf (", %d", arr[i]);
  }
  printf ("\n");
}

void print_line_info(LineInfo *line, char* file_name){

  printf ("%s:%-2lu ", file_name, line->parts->num);

  switch (line->type_t) {
    case str_l:
      printf (CYN "string: " RESET "%s\n", line->str.content);
      break;
    case data_l:
      print_data (line->data.arr, line->data.len);
      break;
    case ext_l:
      printf (CYN "extern: " RESET "%s\n", line->ext_ent.name);
      break;
    case ent_l:
      printf (CYN "entry: " RESET "%s\n", line->ext_ent.name);
      break;
    case op_l:
      print_op_analyze (line->op);
      break;
    case def_l:
      printf (CYN "define:" RESET " %s=%d\n", line->define.name, line->define
          .val);
      break;
  }
}