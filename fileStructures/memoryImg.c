/*
 Created by OMER on 1/24/2024.
*/


#include "memoryImg.h"
#include "../utils/machineWord.h"

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
  if (target_dc - 1 >= 100) {
    r_error ("adding the variables: ", line, " causes data segment overflow");
    return MEMORY_ERROR;
  }

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


//don't take care of add registers!
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
      word = label_word (symbol->address, symbol->are); //todo
      break;
    case INDEX_ADD:
      symbol = (Symbol*) operand->symbol->data;
      word = label_word (symbol->address, symbol->are);
      success_add = push (code_segment, &word); //add the label
      if (success_add) {
        word = imm_word (operand->val); //add the index
      }
      break;
    case REG_ADD:
      if (operand->type == SRC)
        word = registers_word (operand->val, 0);
      else //TARGET
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
  unsigned short int word;
  void *success_add = NULL;

  //first word
  word = first_word (op->propriety->opcode, op->src.add_mode, op->target
      .add_mode);
  success_add = push (code_segment, &word);

  // both operand are register and they share the second word
  if ((op->src.add_mode == REG_ADD) && (op->target.add_mode == REG_ADD)
  && success_add){
    word = registers_word (op->src.val, op->target.val);
    success_add = push (code_segment, &word);
  }

  else{
    //src word
    if ((op->src.add_mode != NONE_ADD) && success_add) {
      success_add = add_operand_word (code_segment, &op->src);
    }
    //target word
    if ((op->target.add_mode != NONE_ADD) && success_add) {
      success_add = add_operand_word (code_segment, &op->target);
    }
  }

  return success_add;
}

void print_code_segment(vector* code_segment){
  int i;
  unsigned short int *word;
  printf ("\n----------------- code segment -----------------\n");
  for (i = 0; i < code_segment->size; ++i){
    word = (unsigned short int*) get (code_segment, i);
    printf("%04d\t", i+INIT_IC);
    printBinaryWord(*word);
  }
}
