/*
 Created by OMER on 3/6/2024.
*/


#include "analyzer.h"

#define CYN   "\x1B[36m"
#define RESET "\x1B[0m"

/***************** directive_lines *******************/
void init_operand(Operand *operand, Opcode opcode, Operand_Type type,
                  char *sym_buffer){
  operand->type = type;
  operand->add_mode = NONE_ADD;
  operand->param_types = param_types[opcode][type];
  operand->info.symInx.symbol = sym_buffer;
  operand->info.symInx.found = TRUE;
}


void init_op_analyze (op_analyze *op, Opcode opcode, char* scr_sym_buffer,
                      char *target_sym_buffer)
{
  op->opcode = opcode;
  init_operand(&op->src, opcode, SRC, scr_sym_buffer);
  init_operand(&op->target, opcode, TARGET, target_sym_buffer);
}


LineInfo* add_line_to_op_list(LineInfo *line){
  op_analyze *tmp_op;
  LinePart *tmp_part;

  tmp_op = malloc (sizeof (op_analyze));
  if (!tmp_op){
    return NULL;
  }

  tmp_part =  malloc (sizeof (LinePart));
  if (!tmp_part){
    free(tmp_op);
    return NULL;
  }

  *tmp_part = *line->parts;
  *tmp_op = *line->info.op;
  line->info.op = tmp_op;
  line->parts = tmp_part;

  return line;
}

/*

vector *init_op_list(void){
  return create_vector(sizeof (LineInfo));
}

*/



int calc_op_size(op_analyze *op){
  int res = 1; /*for the first word */
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
      printf ("<imm: %d>\t", operand->info.imm);
      break;
    case DIRECT_ADD:
      printf ("<symbol: %s>\t", ((Node *)operand->info.symInx.symbol)->word);
      break;
    case INDEX_ADD:
      printf ("<index: %s[%d]>\t", ((Node *)operand->info.symInx.symbol)->word,
      operand->info.symInx.offset);
      break;
    case REG_ADD:
      printf ("<reg: r%d>\t", operand->info.reg_num);
      break;
    case NONE_ADD:
      printf ("           \t");
      break;
  }
}

void print_op_analyze (op_analyze *op)
{
  printf ("%04d\t", op->address);
  printf ("<op: %s>\t", op_names[op->opcode]);
  printf ("<opcode: %d>\t", op->opcode);
  print_operand(&(op->src));
  print_operand(&(op->target));
  printf ("\n");
}

void print_op_list(vector *op_list, char* file_name){
  size_t i;
  op_analyze *op;
  printf ("\n----------------- op list ---------------------\n");
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

  switch (line->type_l) {
    case str_l:
      printf (CYN "string: " RESET "%s\n", line->info.str.content);
      break;
    case data_l:
      print_data (line->info.data.arr, line->info.data.len);
      break;
    case ext_l:
      printf (CYN "extern: " RESET "%s\n", line->info.ext_ent.name);
      break;
    case ent_l:
      printf (CYN "entry: " RESET "%s\n", line->info.ext_ent.name);
      break;
    case op_l:
      print_op_analyze (line->info.op);
      break;
    case def_l:
      printf (CYN "define:" RESET " %s=%d\n", line->info.define.name,
              line->info.define
          .val);
      break;
  }
}