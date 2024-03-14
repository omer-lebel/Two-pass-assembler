/*
 Created by OMER on 1/24/2024.
*/


#include "memoryImg.h"

/****************** data segment *******************/


vector *init_data_seg (size_t *DC)
{
  *DC = 0;
  return create_vector (sizeof (DsWord));
}

exit_code add_to_data_seg (vector *data_segment, size_t *DC,
                           LineInfo *line, DsType type, void *arr, size_t size)
{
  DsWord word;
  int *intArr;
  char *charArr;
  size_t target_dc = *DC + size;

  switch (type) {
    case INT_TYPE:
      intArr = (int *) arr;
      for (; *DC < target_dc; (*DC)++) {
        word.type = type;
        word.val = *intArr++;
        if (!push (data_segment, &word)) {
          return MEMORY_ERROR;
        }
      }
      break;
    case CHAR_TYPE:
      charArr = (char *) arr;
      for (; *DC < target_dc; (*DC)++) {
        word.type = type;
        word.val = (int) *charArr++;
        if (!push (data_segment, &word)) {
          return MEMORY_ERROR;
        }
      }
      break;
    default:
      r_error ("unknown data type in ", line, "");
      return ERROR;
  }
  return SUCCESS;
}

void print_data_segment (vector *data_segment, size_t curr_DC)
{
  size_t i;
  char c;
  DsWord *word;
  printf ("\n----------------- data segment -----------------\n");
  for (i = 0; i < curr_DC; ++i) {
    word = (DsWord *) get (data_segment, i);
    if (word->type == INT_TYPE) {
      printf ("| %d ", word->val);
    }
    else if ((c = (char) word->val) != '\0') { /* char */
      printf ("| %c ", c);
    }
    else { /* '\0' */
      printf ("|   ");
    }
  }
  if (i != 0) {
    printf ("|\n");
  }
}

/****************** code segment *******************/

vector *init_code_seg (size_t IC)
{
  return create_n_vector (MACHINE_WORD_SIZE, IC);
}


/* don't take care of add 2 registers! */
void *add_operand_word (vector *code_segment, Operand *operand)
{
  unsigned short int word;
  vector *success_add = NULL;
  Symbol *symbol;

  switch (operand->add_mode) {
    case IMM_ADD:
      word = imm_word (operand->val);
      break;
    case DIRECT_ADD:
      symbol = (Symbol*) operand->symbol->data;
      word = label_word (symbol->address, symbol->are); /* todo */
      break;
    case INDEX_ADD:
      symbol = (Symbol*) operand->symbol->data;
      word = label_word (symbol->address, symbol->are);
      success_add = push (code_segment, &word); /* add the label */
      if (success_add) {
        word = imm_word (operand->val); /* add the index */
      }
      break;
    case REG_ADD:
      if (operand->type == SRC)
        word = registers_word (operand->val, 0);
      else /* TARGET */
        word = registers_word (0, operand->val);
      break;
    case NONE_ADD:
      break;
  }
  success_add = push (code_segment, &word);
  return success_add;
}

void *add_to_code_seg (vector *code_segment, op_analyze *op)
{
  unsigned short int word, scr_code, target_code;
  void *success_add = NULL;

  /* first word */
  scr_code = (op->src.add_mode == NONE_ADD) ? 0 : op->src.add_mode;
  target_code = (op->target.add_mode == NONE_ADD) ? 0 : op->target.add_mode;
  word = first_word (op->propriety->opcode, scr_code, target_code);
  success_add = push (code_segment, &word);

  /*  both operand are register and they share the second word */
  if ((op->src.add_mode == REG_ADD) && (op->target.add_mode == REG_ADD)
  && success_add){
    word = registers_word (op->src.val, op->target.val);
    success_add = push (code_segment, &word);
  }

  else{
    /* src word */
    if ((op->src.add_mode != NONE_ADD) && success_add) {
      success_add = add_operand_word (code_segment, &op->src);
    }
    /* target word */
    if ((op->target.add_mode != NONE_ADD) && success_add) {
      success_add = add_operand_word (code_segment, &op->target);
    }
  }

  return success_add;
}

void print_code_segment_binary(vector* code_segment){
  size_t i;
  unsigned short int *word;
  printf ("\n----------------- code segment -----------------\n");
  for (i = 0; i < code_segment->size; ++i){
    word = (unsigned short int*) get (code_segment, i);
    printf("%04lu\t", i + IC_START);
    print_binary_word (*word);
  }
}

void print_code_segment(vector* code_segment){
  size_t i;
  unsigned short int *word;
  printf ("\n----------------- code segment -----------------\n");
  for (i = 0; i < code_segment->size; ++i){
    word = (unsigned short int*) get (code_segment, i);
    printf("%04lu\t", i + IC_START);
    print_special_base_word (*word, stdin);
  }
}

void print_memory_img(vector* code_segment, vector* data_segment,
                      FILE *stream){
  size_t i, mem_ind = IC_START;
  size_t len = code_segment->size + data_segment->size + IC_START;
  unsigned short int *word;
  DsWord *data_seg_word;

  fprintf(stream, "%4lu %lu\n", code_segment->size, data_segment->size);

  /* code */
  for (i = 0; i < code_segment->size; ++i, ++mem_ind){
    word = (unsigned short int*) get (code_segment, i);
    fprintf(stream, "%04lu ", mem_ind);
    print_special_base_word (*word, stream);
    if (mem_ind < len - 1){ /*todo check about condition */
      fputc ('\n', stream);
    }
  }
  /* data */
  for (i=0; i < data_segment->size; ++i, ++mem_ind){
    data_seg_word = (DsWord*) get (data_segment, i);
    fprintf(stream, "%04lu ", mem_ind);
    print_special_base_word (data_seg_word->val, stream);
    if (mem_ind < len - 1){
      fputc ('\n', stream);
    }
  }
}