/*
 Created by OMER on 3/5/2024.
*/


#include "secondPass.h"
#include "utils/text.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/ast.h"
#include "fileStructures/memoryImg.h"
#include "fileStructures/entryTable.h"
#include "fileStructures/externTable.h"

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

void* extern_handler_second_pass(char *name, size_t address,
                                     vector* extern_table){
  size_t *last_use;
  ExternSyb *extern_syb =  find_extern_syb(extern_table, name);
  if (!extern_syb){
    return add_to_extern_table (extern_table, name, address);
  }
  else{
    last_use = get_tail (extern_syb->location);
    if (*last_use == address){
      return extern_syb;
    }
    else{
      return add_location (extern_syb, address);
    }
  }
}

exit_code symbol_handler_second_pass (Operand *operand, LineInfo *line,
                                      LinkedList *symbol_table, vector*
                                      extern_table)
{
  Symbol *symbol;
  operand->symbol = findNode (symbol_table, operand->symbol_name);
  if (!operand->symbol) {
    r_error ("undeclared symbol", line, ""); //todo bolt the error token
    return ERROR;
  }

  symbol = (Symbol *) operand->symbol;
  if (symbol->are == EXTERNAL) {
    if (!extern_handler_second_pass(operand->symbol_name, symbol->address,
                               extern_table)){
      return MEMORY_ERROR;
    }
  }
  return SUCCESS;
}

exit_code second_line_process (op_analyze *op, LinkedList *symbol_table,
                               vector *extern_table, vector *code_segment)
{
  exit_code res = SUCCESS;

  //src has symbol
  if (op->src.add_mode != NONE_ADD) {
    if (op->src.add_mode == DIRECT_ADD || op->src.add_mode == INDEX_ADD) {
      res = symbol_handler_second_pass (&op->src, op->line_info,
                                        symbol_table, extern_table);
    }
  }
  //target has symbol
  if (op->src.add_mode != NONE_ADD && res == SUCCESS) {
    if ((op->target.add_mode == DIRECT_ADD
         || op->target.add_mode == INDEX_ADD)) {
      res = symbol_handler_second_pass (&op->target, op->line_info,
                                        symbol_table, extern_table);
    }
  }

  //add to data seg
  if (res == SUCCESS) {
    if (!add_to_code_seg (code_segment, op)) {
      return MEMORY_ERROR;
    }
  }
  return SUCCESS;
}

void *init_second_pass (file_analyze *file_analyze)
{
  file_analyze->code_segment = init_code_seg (file_analyze->IC);
  return file_analyze->code_segment;
}

exit_code process_entry_table (vector *entry_table, LinkedList *symbol_table)
{
  int i;
  EntrySyb *entry_syb;
  Node *node = NULL;
  Symbol *symbol;
  exit_code res = SUCCESS;
  for (i = 0; i < entry_table->size; ++i) {
    entry_syb = (EntrySyb *) get (entry_table, i);
    node = findNode (symbol_table, entry_syb->name);

    //the label is not exist
    if (!node) {
      r_error ("", entry_syb->line_info, " undeclared");
      res += ERROR;
    }

      //label is declare both as extern and as entry label
    else {
      symbol = (Symbol *) node->data;
      if (symbol->are == EXTERNAL) {
        r_error ("", entry_syb->line_info, " declared as both external and entry");
        res += ERROR;
      }
      else{
        entry_syb->address = symbol->address;
      }
    }
  }
  return res;
}

int secondPass (file_analyze *f)
{
  int i;
  op_analyze *op;
  exit_code res = SUCCESS;
  if (!init_second_pass (f)) {
    return EXIT_FAILURE; /* memory error; */
  }

  update_data_symbol_addresses (f->symbol_table, f->IC);
  printf ("\n----------------- symbol table -----------------\n"); //todo delet
  printList (f->symbol_table, stdout);

  // 1) updates labels. 2) convert to binary. 3) update extern uses
  for (i = 0; i < f->op_list->size && res == SUCCESS; ++i) {
    op = (op_analyze *) get (f->op_list, i);
    res = second_line_process (op, f->symbol_table, f->extern_table,
                               f->code_segment);
  }

  //check that all entry symbol are valid
  if (res == SUCCESS) {
    res = process_entry_table (f->entry_table, f->symbol_table);
  }

  print_extern_table (f->extern_table);
  print_entry_table (f->entry_table, f->file_name);
  print_code_segment (f->code_segment);

  freeList (f->symbol_table);
  free_vector (f->data_segment);
  free_vector (f->code_segment);
  free_op_list (f->op_list);
  free_entry_table (f->entry_table);
  free_extern_table (f->entry_table);


  return res == SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}