/*
 Created by OMER on 1/24/2024.
*/


#include "memoryImg.h"
#include "../machineWord.h"

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
    return FAILURE;
  }

  switch (type) {
    case INT_TYPE:
      intArr = (int *) arr;
      for (; *DC < target_dc; (*DC)++) {
        word.type = type;
        word.val = *intArr++;
        if (!push (data_segment, &word)) {
          return FAILURE;
        }
      }
      break;
    case CHAR_TYPE:
      charArr = (char *) arr;
      for (; *DC < target_dc; (*DC)++) {
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
  for (i = 0; i < code_segment->size; ++i){
    word = (unsigned short int*) get (code_segment, i);
    printf("%04d\t", i+INIT_IC);
    printBinaryWord(*word);
  }
}


/***************** directive_lines *******************/
vector *init_op_list(void){
  return create_vector (sizeof (op_analyze));
}

void copy_line_info(LineInfo *dst, LineInfo *src){
  strcpy(dst->prefix, src->prefix);
  strcpy(dst->token, src->file);
  strcpy(dst->postfix, src->postfix);
  dst->num = src->num;
}

op_analyze *add_to_op_list(vector* op_list, op_analyze *op){
  LineInfo *tmp = malloc (sizeof (LineInfo));
  if (!tmp){
    return NULL;
  }
  copy_line_info(tmp, op->line_info);
  op->line_info = tmp;
  return push (op_list, op);;
}

size_t calc_op_size(op_analyze *op){
  size_t res = 1; //for the first word
  Addressing_Mode mode;

  //special case of 2 registers that share the same word
  if (op->src.add_mode == REG_ADD && op->target.add_mode == REG_ADD){
    res+=1;
  }
  else{
    //word for src operand
    mode = op->src.add_mode;
    if (mode != NONE_ADD){
      res += (mode == INDEX_ADD ? 2 : 1);
    }
    //word for target operand
    mode = op->target.add_mode;
    if (mode != NONE_ADD){
      res += (mode == INDEX_ADD ? 2 : 1);
    }
  }
  return res;
}

void print_operand(Operand *operand){
  char symbol[MAX_LINE_SIZE] = "?";
  if (operand->symbol != NULL) {
    strcpy (symbol, operand->symbol->word);
  }

  switch (operand->add_mode) {
    case IMM_ADD:
      printf ("<imm: %d>\t", operand->val);
      break;
    case DIRECT_ADD:
      printf ("<symbol: %s>\t", symbol);
      break;
    case INDEX_ADD:
      printf ("<index: %s[%d]>\t", symbol, operand->val);
      break;
    case REG_ADD:
      printf ("<reg: r%d>\t", operand->val);
      break;
    case NONE_ADD:
      break;
  }
}

void print_op_analyze (op_analyze *op, char* file_name)
{
  if (op->errors == TRUE) {
    return;
  }
  printf ("%s:%-2lu ", file_name, op->line_info->num);
  printf ("<op: %s>\t", op->propriety->name);
  printf ("<opcode: %d>\t", op->propriety->opcode);
  print_operand(&(op->src));
  print_operand(&(op->target));
  printf ("\n");
}

void print_op_list(vector *op_list, char* file_name){
  int i;
  op_analyze *op;
  for (i=0; i<op_list->size; ++i){
    op = (op_analyze*) get (op_list, i);
    print_op_analyze (op, file_name);
  }
}

void free_op_list(vector *op_list){
  int i;
  op_analyze *op;
  for (i=0; i<op_list->size; ++i){
    op = (op_analyze*) get(op_list,i);
    free(op->line_info);
  }
  free_vector (op_list);
}