#include "externUsages.h"


typedef Node Extern_Syb;
typedef LinkedList Extern_List;
typedef vector Usages_List;

/* ---------------------- helper function declaration ---------------------- */
void *init_usages_list (const void *first_usage);
Usages_List *new_usages_list (void);
void print_usages_list (const char *ext_name, const void *data, FILE*stream);
void free_usages_list (void *usages_list);
exit_code build_extern_table (Extern_List *extern_list, Op_List *op_list, char *file_name);
Symbol *extern_usages_in_operand (Operand *operand);
int calc_extern_location (Op_Analyze *op, Operand_Type type);
Bool add_new_usages (Extern_List *extern_list, char *ext_name, int line_num);

/* ------------------------------------------------------------------------- */

exit_code print_extern_table (Op_List *op_list, FILE *output, char *file_name)
{
  exit_code res;
  Extern_List *extern_list = create_list (init_usages_list, print_usages_list,
                                          free_usages_list);
  if (!extern_list) {
    return MEMORY_ERROR;
  }

  res = build_extern_table (extern_list, op_list, file_name);
  if (res != SUCCESS) {
    free_list (extern_list);
    return res;
  }

  print_list (extern_list, output);

  free_list (extern_list);

  return SUCCESS;
}

/* ------------------ function for the link list */
/**
 * @brief Initializes a usage list with the first usage line number.
 *
 * @param first_usage Pointer to the first usage line number.
 * @return Pointer to the initialized usage list if successful,
 *         or NULL if memory allocation fails.
 */
void *init_usages_list (const void *first_usage)
{
  int *first = (int *) first_usage;
  Usages_List *usages_list = new_usages_list ();
  if (!usages_list) {
    return NULL;
  }
  if (!push (usages_list, first)) {
    return NULL;
  }
  return usages_list;
}

/**
 * @brief Creates a new usage list for storing line numbers associated with
 * external symbols.
 *
 * @return Pointer to the newly created usage list if successful,
 *          or NULL if memory allocation fails.
 */
Usages_List *new_usages_list (void)
{
  return create_vector (sizeof (int), NULL, NULL);
}

/**
 * @brief Prints the usage list associated with an external symbol.
 *
 * @param ext_name Name of the external symbol.
 * @param data Pointer to the usage list data.
 * @param stream File stream to print to.
 */
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

/**
 * @brief Frees the memory allocated for a usage list.
 *
 * @param usages_list Pointer to the usage list to free.
 */
void free_usages_list (void *usages_list)
{
  free_vector (usages_list);
}

/* ------------------ end of function for the link list */

/**
 * @brief Builds the table of external symbols and their usages.
 *
 * @param extern_list Pointer to the list of external symbols.
 * @param op_list Pointer to the list of operations.
 * @param file_name Name of the file.
 * @return The exit code indicating the success or failure of the operation.
 */
exit_code build_extern_table (Extern_List *extern_list, Op_List *op_list,
                              char *file_name)
{
  Op_Line *op_line;
  int i = 0, memInx = -1;
  Symbol *src_symbol = NULL, *target_symbol = NULL;
  while ((op_line = get (op_list, i++))) {
    /* src */
    if ((src_symbol = extern_usages_in_operand (&op_line->analyze->src))) {
      memInx = calc_extern_location (op_line->analyze, SRC);
      if (!add_new_usages (extern_list, src_symbol->word, memInx)) {
        return MEMORY_ERROR;
      }
    }
    /* target */
    if ((target_symbol = extern_usages_in_operand (&op_line->analyze->target))
        && target_symbol != src_symbol) {
      memInx = calc_extern_location (op_line->analyze, TARGET);
      if (!add_new_usages (extern_list, target_symbol->word, memInx)) {
        return MEMORY_ERROR;
      }
    }
  }
  if (memInx < 0) {
    remove_file (file_name, "ext");
    return ERROR;
  }
  return SUCCESS;
}

/**
 * @brief Determines if an operand refers to an external symbol.
 *
 * @param operand Pointer to the operand.
 * @return Pointer to the external symbol if found, or NULL otherwise.
 */
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

/**
 * @brief Calculates the location of an external symbol in memory.
 *
 * @param op Pointer to the operation being analyzed.
 * @param type Type of operand (source or target).
 * @return The calculated memory location of the external symbol.
 */
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

/**
 * @brief Adds a new usage line number to the list associated with an
 * external symbol.
 *
 * @param extern_list Pointer to the list of external symbols.
 * @param ext_name Name of the external symbol.
 * @param line_num Line number of the usage.
 * @return TRUE if the operation is successful, FALSE otherwise.
 */
Bool add_new_usages (Extern_List *extern_list, char *ext_name, int line_num)
{
  Extern_Syb *extern_syb = find_node (extern_list, ext_name);
  if (extern_syb) {
    if (!push (extern_syb->data, &line_num)) {
      return FALSE;
    }
    return TRUE;
  }
  /* else new extern symbol */
  extern_syb = create_node (extern_list, ext_name, &line_num);
  if (!extern_syb) {
    return FALSE;
  }
  append_to_tail (extern_list, extern_syb);
  return TRUE;
}
