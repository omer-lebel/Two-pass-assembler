/*
 Created by OMER on 3/5/2024.
*/


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

/* todo think of better place? maybe main or setting */
void free_file_analyze2 (file_analyze *f)
{
  if (f->symbol_table) {
    freeList (f->symbol_table);
  }

  if (f->data_segment) {
    free_vector (f->data_segment);
  }

  if (f->code_segment) {
    free_vector (f->code_segment);
  }

  if (f->op_list) {
    free_op_list (f->op_list);
  }

  if (f->entry_table) {
    free_entry_table (f->entry_table);
  }

  if (f->extern_table) {
    free_extern_table (f->extern_table);
  }

  memset (f, 0, sizeof (file_analyze));
}

size_t calc_extern_location (op_analyze *op, Operand_Type type)
{
  int offset;
  if (type == SRC || op->src.add_mode == NONE_ADD) {
    offset = 1;
  }
  else if (op->src.add_mode != INDEX_ADD) {
    offset = 2;
  }
  else { /* op->src.add_mode == INDEX_ADD */
    offset = 3;
  }
  return op->address + offset;
}

void *extern_handler_second_pass (op_analyze *op, Operand_Type type,
                                  vector *extern_table)
{
  Operand *operand = (type == SRC ? &op->src : &op->target);
  ExternSyb *extern_syb = find_extern_syb (extern_table, operand->symbol_name);
  size_t address = calc_extern_location (op, type);
  if (!extern_syb) {
    return add_to_extern_table (extern_table, operand->symbol_name, address);
  }
  else {
    return add_location (extern_syb, address);
  }
}

exit_code symbol_handler_second_pass (op_analyze *op, Operand_Type type,
                                      LinkedList *symbol_table,
                                      vector *extern_table)
{
  Node *node = NULL;
  Symbol *symbol = NULL;
  Operand *operand = (type == SRC ? &op->src : &op->target);
  node = findNode (symbol_table, operand->symbol_name);
  if (!node) {
    r_error ("undeclared symbol", op->line_part, "");
    /* todo bolt the error token */
    return ERROR;
  }

  symbol = (Symbol *) node->data;
  if (symbol->are == EXTERNAL) {
    if (!extern_handler_second_pass (op, type, extern_table)) {
      return MEMORY_ERROR;
    }
  }
  operand->symbol = node;
  return SUCCESS;
}

exit_code second_line_process (op_analyze *op, LinkedList *symbol_table,
                               vector *extern_table, vector *code_segment)
{
  exit_code res = SUCCESS;

  /* src has symbol */
  if (op->src.add_mode != NONE_ADD) {
    if (op->src.add_mode == DIRECT_ADD || op->src.add_mode == INDEX_ADD) {
      res = symbol_handler_second_pass (op, SRC, symbol_table, extern_table);
    }
  }
  /* target has symbol */
  if (op->target.add_mode != NONE_ADD && res == SUCCESS) {
    if ((op->target.add_mode == DIRECT_ADD
         || op->target.add_mode == INDEX_ADD)) {
      res = symbol_handler_second_pass (op, TARGET, symbol_table, extern_table);
    }
  }

  /* add to data seg */
  if (res == SUCCESS) {
    if (!add_to_code_seg (code_segment, op)) {
      return MEMORY_ERROR;
    }
  }
  return res;
}

/* todo change */
exit_code init_second_pass (file_analyze *f)
{
  f->code_segment = init_code_seg (f->IC);
  f->extern_table = init_extern_table ();

  if (!f->code_segment || !f->extern_table) {
    free_file_analyze2 (f);
    return MEMORY_ERROR;
  }
  return SUCCESS;
}

exit_code process_entry_table (vector *entry_table, LinkedList *symbol_table)
{
  size_t i;
  EntrySyb *entry_syb;
  Node *node = NULL;
  Symbol *symbol;
  exit_code res = SUCCESS;
  for (i = 0; i < entry_table->size; ++i) {
    entry_syb = (EntrySyb *) get (entry_table, i);
    node = findNode (symbol_table, entry_syb->name);

    /* the label is not exist */
    if (!node) {
      r_error ("", entry_syb->line_info, " undeclared");
      res += ERROR;
    }

      /* label is declare both as extern and as entry label */
    else {
      symbol = (Symbol *) node->data;
      if (symbol->are == EXTERNAL) {
        r_error ("", entry_syb->line_info, " declared as both external and entry");
        res += ERROR;
      }
      else {
        entry_syb->address = symbol->address;
      }
    }
  }
  return res;
}

exit_code write_analyze(file_analyze *f){
  FILE *ob_file, *ext_file, *ent_file;

  if (f->data_segment->size > 0 || f->code_segment->size > 0){
    ob_file = open_file(f->file_name, ".ob", "w");
    if (!(ob_file)){
      return ERROR;
    }
    print_memory_img(f->code_segment, f->data_segment, ob_file);
    fclose (ob_file);
  }

  if (f->entry_table->size > 0){
    ent_file = open_file(f->file_name, ".ent", "w");
    if (!(ent_file)){
      return ERROR;
    }
    print_entry_table(f->entry_table, ent_file);
    fclose (ent_file);
  }

  if (f->extern_table->size > 0){
    ext_file = open_file(f->file_name, ".ext", "w");
    if (!(ext_file)){
      return ERROR;
    }
    print_extern_table (f->extern_table, ext_file);
    fclose (ext_file);
  }

  return SUCCESS;
}

exit_code secondPass (file_analyze *f)
{
  size_t i;
  op_analyze *op;
  exit_code res = SUCCESS;

  update_data_symbol_addresses (f->symbol_table, f->IC);
  printf ("\n----------------- symbol table -----------------\n"); 
  /* todo delete */
  printList (f->symbol_table, stdout);
  res = init_second_pass (f);

  /*  1) updates labels. 2) convert to binary. 3) update extern uses */
  for (i = 0; i < f->op_list->size && res != MEMORY_ERROR; ++i) {
    op = (op_analyze *) get (f->op_list, i);
    res = second_line_process (op, f->symbol_table, f->extern_table,
                               f->code_segment);
    f->error += res;
  }

  /* check that all entry symbol are valid */
  if (res != MEMORY_ERROR) {
    res = process_entry_table (f->entry_table, f->symbol_table);
  }

  if (f->error == SUCCESS){
    write_analyze (f);
  }

  free_file_analyze2 (f);

  return res;
}