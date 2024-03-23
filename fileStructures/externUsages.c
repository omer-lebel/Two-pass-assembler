#include "externUsages.h"
typedef Node Extern_Syb;
typedef LinkedList Extern_List;
typedef vector Usages_List;


Usages_List *new_usages_list (void)
{
  return create_vector (sizeof (int), NULL, NULL);
}

void *init_usages_list (const void *first_usage)
{
  int *first = (int *) first_usage;
  Usages_List *usages_list = new_usages_list ();
  if (!usages_list) {
    return NULL;
  }
  if (!push (usages_list, first)){
    return NULL;
  }
  return usages_list;
}
typedef void (*free_data_func) (void *);

void print_usages_list (const char *ext_name, const void *data, FILE *stream)
{
  int i, *line_num;
  Usages_List *usages_list = (Usages_List *) data;
  for (i = 0; i < usages_list->size; ++i) {
    line_num = (int *) get (usages_list, i);
    fprintf (stream, "%s\t%04d", ext_name, *line_num);
    if (i < usages_list->size)
      fputc ('\n', stream);
  }
}

void free_usages_list (void *usages_list)
{
  return free_vector (usages_list);
}

Symbol *extern_usages_in_operand (Operand *operand)
{
  Symbol *symbol;
  Symbol_Data *symbol_data;
  Addressing_Mode addr_mode = operand->add_mode;

  if (addr_mode == DIRECT_ADD || addr_mode == INDEX_ADD) {
    symbol = (Symbol *) operand->info.symInx.symbol;
    symbol_data = (Symbol_Data *) symbol->data;

    if (symbol_data->type == EXTERN) {
      return symbol;
    }
  }
  return NULL;
}

int calc_extern_location (Op_Analyze *op, Operand_Type type)
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


Bool
add_new_usages (Extern_List *extern_list, char *ext_name, int line_num)
{
  Extern_Syb *extern_syb = findNode (extern_list, ext_name);
  if (extern_syb) {
    if (!push (extern_syb->data, &line_num)) {
      return FALSE;
    }
    return TRUE;
  }
  /* else new extern symbol */
  extern_syb = createNode (extern_list, ext_name, &line_num);
  if (!extern_syb) {
    return FALSE;
  }
  appendToTail(extern_list, extern_syb);
  return TRUE;
}

exit_code build_extern_table (Extern_List *extern_list, Op_List *op_list,
                         char* file_name)
{
  Op_Line *op_line;
  int i = 0, memInx = -1;
  Symbol *src_symbol = NULL, *target_symbol = NULL;
  while ((op_line = get (op_list, i++))) {
    /* src */
    if ((src_symbol = extern_usages_in_operand (&op_line->analyze->src))) {
      memInx = calc_extern_location (op_line->analyze, SRC);
      if (!add_new_usages(extern_list, src_symbol->word, memInx)){
        return MEMORY_ERROR;
      }
    }
    /* target */
    if ((target_symbol = extern_usages_in_operand (&op_line->analyze->target))
        && target_symbol != src_symbol) {
      memInx = calc_extern_location (op_line->analyze, TARGET);
      if (!add_new_usages(extern_list, target_symbol->word, memInx)){
        return MEMORY_ERROR;
      }
    }
  }
  if (memInx < 0){
    remove_file (file_name, "ext");
    return ERROR;
  }
  return SUCCESS;
}


exit_code print_extern_table(Op_List *op_list, FILE *output, char *file_name)
{
  exit_code res;
  Extern_List *extern_list = createList (init_usages_list, print_usages_list,
                                   free_usages_list);
  if (!extern_list){
    return MEMORY_ERROR;
  }

  res = build_extern_table(extern_list, op_list, file_name);
  if (res != SUCCESS){
    freeList (extern_list);
    return res;
  }

  printList (extern_list, output);

  freeList (extern_list);

  return SUCCESS;
}

