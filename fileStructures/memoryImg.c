/*
 Created by OMER on 1/24/2024.
*/


#include "memoryImg.h"
#include "../machineWord.h"


/****************** data segment *******************/


vector *init_data_seg (size_t *curr_DC)
{
  *curr_DC = 0;
  return create_vector (sizeof (DsWord));
}

exit_code add_to_data_seg (vector *data_segment, size_t *curr_DC,
                           LineInfo *line, DsType type, void *arr, size_t size)
{
  DsWord word;
  int *intArr;
  char *charArr;
  size_t target_dc = *curr_DC + size;
  if (target_dc - 1 >= 100) {
    r_error ("adding the variables: ", line, " causes data segment overflow");
    return FAILURE;
  }

  switch (type) {
    case INT_TYPE:
      intArr = (int *) arr;
      for (; *curr_DC < target_dc; (*curr_DC)++) {
        word.type = type;
        word.val = *intArr++;
        if (!push (data_segment, &word)) {
          return FAILURE;
        }
      }
      break;
    case CHAR_TYPE:
      charArr = (char *) arr;
      for (; *curr_DC < target_dc; (*curr_DC)++) {
        word.type = type;
        word.val = (int) *charArr++;
        if (!push (data_segment, &word)) {
          return FAILURE;
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

/****************** text segment *******************/

vector *init_text_seg (size_t *curr_IC)
{
  *curr_IC = 100;
  return create_vector (MACHINE_WORD_SIZE);
}

//don't take care of add registers!
void *add_operand_word (vector *code_segment, Operand *operand)
{
  unsigned short int word;
  vector *success_add = NULL;
  Symbol *symbol = (Symbol *) operand->symbol;

  switch (operand->add_mode) {
    case IMM_ADD:
      word = imm_word (operand->val);
      break;
    case DIRECT_ADD:
      word = label_word (symbol->address, operand->seen); //todo
      break;
    case INDEX_ADD:
      word = label_word (symbol->address, operand->seen);
      success_add = push (code_segment, &word);
      if (success_add) {
        word = imm_word (operand->val);
        success_add = push (code_segment, &word);
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

  //src word
  else if ((op->src.add_mode != NONE_ADD) && success_add) {
    success_add = add_operand_word (code_segment, &op->src);
  }
  //target word
  if ((op->target.add_mode != NONE_ADD) && success_add) {
    success_add = add_operand_word (code_segment, &op->src);
  }

  return success_add;
}

void print_op_analyze (op_analyze *op)
{
  char symbol_src[MAX_LINE_SIZE] = "?";
  char symbol_target[MAX_LINE_SIZE] = "?";
  if (op->errors == TRUE) {
    return;
  }

  if (op->src.symbol != NULL) {
    strcpy (symbol_src, op->src.symbol->word);
  }

  if (op->target.symbol != NULL) {
    strcpy (symbol_target, op->target.symbol->word);
  }

  printf ("%s:%-2lu ", op->line_info->file, op->line_info->num);
  printf ("<op: %s>\t", op->propriety->name);
  printf ("<opcode: %d>\t", op->propriety->opcode);

  switch (op->src.add_mode) {
    case IMM_ADD:
      printf ("<imm: %d>\t", op->src.val);
      break;
    case DIRECT_ADD:
      printf ("<symbol: %s>\t", symbol_src);
      break;
    case INDEX_ADD:
      printf ("<index: %s[%d]>\t", symbol_src, op->src.val);
      break;
    case REG_ADD:
      printf ("<reg: r%d>\t", op->src.val);
      break;
    case NONE_ADD:
      break;
  }

  switch (op->target.add_mode) {
    case IMM_ADD:
      printf ("<imm: %d>\t", op->target.val);
      break;
    case DIRECT_ADD:
      printf ("<symbol: %s>\t", symbol_target);
      break;
    case INDEX_ADD:
      printf ("<index: %s[%d] >\t", symbol_target, op->target.val);
      break;
    case REG_ADD:
      printf ("<reg: r%d>\t", op->target.val);
      break;
    case NONE_ADD:
      break;
  }
  printf ("\n");
}
