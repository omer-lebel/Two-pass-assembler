/*
 Created by OMER on 1/24/2024.
*/


#include "memoryImg.h"

/****************** data segment *******************/


Bool add_to_data_segment (Data_Segment *data_segment, int *DC,
                          DsType type, void *arr, int size)
{
  DsWord word;
  int *intArr;
  char *charArr;
  int target_dc = *DC + size;

  switch (type) {
    case INT_TYPE:
      intArr = (int *) arr;
      for (; *DC < target_dc; (*DC)++) {
        word.type = type;
        word.val = *intArr++;
        if (!push (data_segment, &word)) {
          return FALSE;
        }
      }
      break;
    case CHAR_TYPE:
      charArr = (char *) arr;
      for (; *DC < target_dc; (*DC)++) {
        word.type = type;
        word.val = (int) *charArr++;
        if (!push (data_segment, &word)) {
          return FALSE;
        }
      }
      break;
  }
  return TRUE;
}

void print_data_word(const void* elem, FILE *stream){
  char c;
  DsWord *ds_word = (DsWord*) elem;
  if (ds_word->type == INT_TYPE) {
    fprintf (stream, " %d ", ds_word->val);
  }
  else if ((c = (char) ds_word->val) != '\0') { /* char */
    fprintf (stream, " %c ", c);
  }
  else { /* '\0' */
    fprintf (stream, "   ");
  }
}

Data_Segment *new_data_segment (int *DC)
{
  *DC = 0;
  return create_vector (sizeof (DsWord), NULL, print_data_word, NULL);
}


void show_data_segment (Data_Segment *data_segment, FILE *stream)
{
  fprintf (stream, "\n----------------- data segment -----------------\n");
  print_vector (data_segment, stream, "|", "|\n");
}

void free_data_segment (Data_Segment *data_segment)
{
  free_vector (data_segment);
}

void print_data_segment (Data_Segment *data_segment, int *memInx, int len,
                         FILE *stream)
{
  DsWord *word;
  int i = 0;
  for (; *memInx < len; ++(*memInx), ++i) {
    word = (DsWord *) get (data_segment, i);
    fprintf (stream, "%04d ", *memInx);
    print_special_base_word (word->val, (len - *memInx == 1), stream);
  }
}

/****************** print memory img *******************/

/* don't take care of add 2 registers! */
void print_operand_word (Operand *operand, int *memInx, int len, FILE *stream)
{
  unsigned short int word = 0;
  Symbol_Data *symbol;
  int are = 0;

  switch (operand->add_mode) {
    case IMM_ADD:
      word = imm_word (operand->info.imm);
      break;
    case DIRECT_ADD:
      symbol = (Symbol_Data *) ((Symbol_N *) operand->info.symInx.symbol)->data;
      are = symbol->type == EXTERN ? external_b : relocatable_b;
      word = label_word (symbol->val, are);
      break;
    case INDEX_ADD:
      symbol = (Symbol_Data *) ((Symbol_N *) operand->info.symInx.symbol)->data;
      word = label_word (symbol->val, are);
      printf ("%04d\t", (*memInx)++); /* print the label */
      print_special_base_word (word, (len - *memInx == 1), stream);
      word = imm_word (operand->info.symInx.offset); /* add the index */
      break;
    case REG_ADD:
      if (operand->type == SRC)
        word = registers_word (operand->info.reg_num, 0);
      else /* TARGET */
        word = registers_word (0, operand->info.reg_num);
      break;
    case NONE_ADD: /* never happened */
      return;
  }
  printf ("%04d\t", (*memInx)++);
  print_special_base_word (word, (len - *memInx == 1), stream);
}

void add_op_line_to_code_segment (op_analyze *op, int *memInx, int len, FILE *stream)
{
  unsigned short int word, scr_code, target_code;
  /* first word */
  scr_code = (op->src.add_mode == NONE_ADD) ? 0 : op->src.add_mode;
  target_code = (op->target.add_mode == NONE_ADD) ? 0 : op->target.add_mode;
  word = first_word (op->opcode, scr_code, target_code);
  printf ("%04d\t", (*memInx)++);
  print_special_base_word (word, (len - *memInx == 1), stream);

  /*  both operand are registers, so they share the second word */
  if ((op->src.add_mode == REG_ADD) && (op->target.add_mode == REG_ADD)) {
    word = registers_word (op->src.info.reg_num, op->target.info.reg_num);
    printf ("%04d\t", (*memInx)++);
    print_special_base_word (word, (len - *memInx == 1), stream);
  }

  else {
    /* src word */
    if ((op->src.add_mode != NONE_ADD)) {
      print_operand_word (&op->src, memInx, len, stream);
    }
    /* target word */
    if ((op->target.add_mode != NONE_ADD)) {
      print_operand_word (&op->target, memInx, len, stream);
    }
  }
}

void print_code_segment (Op_List *op_list, int *memInx, int len, FILE *stream)
{
  op_analyze *op;
  int i = 0;
  for (; *memInx < len; ++(*memInx), ++i) {
    op = (op_analyze *) get (op_list, i);
    add_op_line_to_code_segment (op, memInx, len, stream);
  }
}

void print_memory_img (Op_List *op_list, int ic, Data_Segment *data_segment,
                       int dc, FILE *stream)
{
  int len = ic + dc + IC_START;
  int memInx = IC_START;

  fprintf (stream, "%4d %d\n", ic, dc);

  print_code_segment (op_list, &memInx, len, stream);

  print_data_segment (data_segment, &memInx, len, stream);
}

