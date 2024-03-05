//
// Created by OMER on 3/5/2024.
//

#include "secondPass.h"

/*
 * for each line:
 * if contain labels:
 *      1. check valid (exist and suit type)
 *      2. valid? updates address
 *      2. extern? add into extern uses
 * convert to binary
 *
 * for each entry label
 *      1. check validation (exist and not extern)
 *      2. add into entry list
 */

exit_code symbol_handler_second_pass (Operand *operand, LinkedList
*symbol_table)
{
  Symbol *symbol;
  operand->symbol = findNode (symbol_table, operand->symbol_name);
  if (!operand->symbol) {
    return ERROR;
  }

  symbol = (Symbol *) operand->symbol;
  if (symbol->are == EXTERNAL) {
    //add to external list
  }
  return SUCCESS;
}

exit_code second_process (op_analyze *op, LinkedList *symbol_table,
                          vector* code_segment)
{
  exit_code res = SUCCESS;
  LineInfo *line = op->line_info;
//  get_first_operand_tok(op);

  //src has symbol
  if (op->src.add_mode != NONE_ADD){
    if (op->src.add_mode == DIRECT_ADD || op->src.add_mode == INDEX_ADD) {
      res = symbol_handler_second_pass (&op->src ,symbol_table);
    }
  }
  //target has symbol
  if (op->src.add_mode != NONE_ADD && res == SUCCESS){
    if ((op->target.add_mode == DIRECT_ADD
         || op->target.add_mode == INDEX_ADD)) {
      res = symbol_handler_second_pass (&op->target, symbol_table);
    }
  }

  //add to data seg
  if (res == SUCCESS) {
    if (!add_to_code_seg (code_segment, op)) {
      return FAILURE; //memory error
    }
    return SUCCESS;
  }
  return SUCCESS;
}

void *init_second_pass (file_analyze *file_analyze)
{
  file_analyze->code_segment = init_code_seg (file_analyze->IC);
  return file_analyze->code_segment;
}

int secondPass (file_analyze *file_analyze)
{
  int i;
  op_analyze *op;
  exit_code res = SUCCESS;
  if (!init_second_pass (file_analyze)) {
    return EXIT_FAILURE; /* memory error; */
  }

  update_data_symbol_addresses(file_analyze->symbol_table, file_analyze->IC);
  printList (file_analyze->symbol_table, stdout);

  // labels and convert and extern
  for (i = 0; i < file_analyze->op_list->size && res == SUCCESS; ++i) {
    op = (op_analyze *) get (file_analyze->op_list, i);
    res = second_process (op, file_analyze->symbol_table,
                          file_analyze->code_segment);
  }

  print_code_segment(file_analyze->code_segment);


  freeList (file_analyze->symbol_table);
  free_vector (file_analyze->data_segment);
  free_op_list (file_analyze->op_list);

  return EXIT_SUCCESS;
}