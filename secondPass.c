/* ------------------------------- includes ------------------------------- */
#include "secondPass.h"
#include "utils/errors.h"
#include "utils/vector.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/codeSeg.h"
#include "fileStructures/dataSeg.h"
#include "fileStructures/entryLines.h"
#include "fileStructures/externUsages.h"
/* ---------------------- helper function declaration ---------------------- */
void        print_usages_errors   (OpLinesList *op_list, char *file_name);
Symbol*     is_unresolved_usages  (Operand *operand);
void        print_entry_errors    (EntryLinesList *entry_list, char *file_name);
exit_code   print_analyze         (file_analyze *f);
void        print_memory_img      (OpLinesList *op_list, int ic, DataSegment *data_segment, int dc, FILE *stream);
/* ------------------------------------------------------------------------- */

exit_code secondPass (file_analyze *f)
{
  exit_code res = ERROR;
  int unresolved_entries = f->symbol_table->unresolved_entry_count;
  int unresolved_usages = f->symbol_table->unresolved_symbols_count;

  int len = (int) strlen (f->file_name);
  strcat (f->file_name, ".am");

  /* only unresolved symbols */
  if (unresolved_entries == 0 && unresolved_usages > 0) {
    print_usages_errors (f->op_list, f->file_name);
  }
  /* only unresolved entries */
  if (unresolved_entries > 0 && unresolved_usages == 0) {
    print_entry_errors (f->entry_list, f->file_name);
  }
  /* both unresolved entries and symbols */
  if (unresolved_entries > 0 && unresolved_usages > 0) {
    print_usages_errors (f->op_list, f->file_name);
    print_entry_errors (f->entry_list, f->file_name);
  }

  /* no errors at all */
  f->file_name[len] = '\0';
  if (unresolved_entries == 0 && unresolved_usages == 0) {
    if (f->error == 0) {
      update_data_symbol_addresses (f->symbol_table, f->IC);
      res = print_analyze (f);
    }
  }

#ifdef DEBUG_SECOND_PASS
  if (res != MEMORY_ERROR){
    display_debug(f, stdout, "first");
  }
#endif

  free_file_analyze (f);
  return res;
}

/**
 * @brief Prints errors related to unresolved symbol usages in operands.
 *
 * Traverses the operator list and identifies unresolved symbol usages in
 * operands. Prints error messages for each unresolved symbol.
 *
 * @param op_list       Pointer to the opcode list.
 * @param file_name     Name of the file being analyzed.
 */
void print_usages_errors (OpLinesList *op_list, char *file_name)
{
  OpLine *op_line;
  int i = 0;
  Symbol *src_symbol = NULL, *target_symbol = NULL;
  while ((op_line = get_op_line (op_list, i++))) {

    /* unresolved symbol in src operand */
    if ((src_symbol = is_unresolved_usages (&op_line->analyze->src))) {
      raise_second_pass_err (op_line->parts, src_symbol->word, file_name);
    }

    /* unresolved symbol in target operand t*/
    if ((target_symbol = is_unresolved_usages (&op_line->analyze->target))) {

      /* print error only if target makes new error for this line  */
      if (target_symbol != src_symbol && !src_symbol) {
        raise_second_pass_err (op_line->parts, target_symbol->word, file_name);
      }
    }
  }
}

/**
 * @brief Check if an operand using ab unresolved symbol
 *
 * Checks if an operand contains an unresolved symbol usage and returns
 * a pointer to the unresolved symbol if found.
 *
 * @param operand   Pointer to the operand to be checked.
 * @return Pointer to the unresolved symbol if found, otherwise NULL.
 */
Symbol *is_unresolved_usages (Operand *operand)
{
  Symbol *symbol;
  Symbol_Data *symbol_data;
  AddressingMode addr_mode = operand->add_mode;

  if (addr_mode == DIRECT_ADD || addr_mode == INDEX_ADD) {
    symbol = (Symbol *) operand->info.symInx.symbol;
    symbol_data = (Symbol_Data *) symbol->data;

    if (symbol_data->type == UNRESOLVED_ENTRY_USAGE
        || symbol_data->type == UNRESOLVED_USAGE) {
      return symbol;
    }
  }
  return NULL;
}

/**
 * @brief Prints errors related to unresolved entry symbols.
 *
 * Traverses the entry list and identifies unresolved entry symbols.
 * Prints error messages for each unresolved symbol.
 *
 * @param entry_list    Pointer to the entry list.
 * @param file_name     Name of the file being analyzed.
 */
void print_entry_errors (EntryLinesList *entry_list, char *file_name)
{
  EntryLine *entry_line;
  Symbol *symbol;
  Symbol_Data *symbol_data;
  int i = 0;

  while ((entry_line = get_entry_line (entry_list, i++))) {

    symbol = entry_line->symbol;
    symbol_data = (Symbol_Data *) symbol->data;

    if (symbol_data->type == UNRESOLVED_ENTRY
        || symbol_data->type == UNRESOLVED_ENTRY_USAGE) {

      raise_second_pass_err (entry_line->parts, entry_line->symbol->word, file_name);
    }
  }
}

/**
 * @brief Prints output files of the assembler analyse
 *
 * Prints the memory image (file.ob), entry file (file.ent), and external
 * file (file.ext)
 *
 * @param f Pointer to the file analysis structure.
 * @return Exit code
 */
exit_code print_analyze (file_analyze *f)
{
  FILE *ob_file, *ent_file, *ext_file;
  exit_code res;

  /* --------- .ob file --------- */
  ob_file = open_file (f->file_name, ".ob", "w");
  if (!(ob_file)) {
    return ERROR;
  }
  print_memory_img (f->op_list, f->IC, f->data_segment, f->DC, ob_file);
  printf (" -- %s.ob created \n", f->file_name);
  fclose (ob_file);

  /* --------- .ent file --------- */
  if (!is_empty (f->entry_list)) {
    ent_file = open_file (f->file_name, ".ent", "w");
    if (!(ent_file)) {
      return ERROR;
    }
    print_entry_list (f->entry_list, ent_file);
    printf (" -- %s.ent created \n", f->file_name);
    fclose (ent_file);
  }

  /* --------- .ext file --------- */
  if (f->symbol_table->extern_count > 0) {
    ext_file = open_file (f->file_name, ".ext", "w");
    if (!(ext_file)) {
      return ERROR;
    }
    res = print_extern_table (f->op_list, ext_file, f->file_name);
    if (res != SUCCESS) {
      fclose (ext_file);
      return res;
    }
    printf (" -- %s.ext created \n", f->file_name);
    fclose (ext_file);
  }
  return SUCCESS;
}

/**
 * @brief Prints the memory image to the output stream (.ob file)
 *
 * Prints the memory image, including the code segment and data segment,
 * to the specified output stream.
 *
 * @param op_list       Pointer to the op list that will be the code segment
 * @param ic            Instruction count (IC).
 * @param data_segment  Pointer to the data segment.
 * @param dc            Data count (DC).
 * @param stream        Pointer to the output stream (file.ob)
 */
void print_memory_img (OpLinesList *op_list, int ic, DataSegment *data_segment,
                       int dc, FILE *stream)
{

  fprintf (stream, "%4d %d\n", ic, dc);

  if (ic > 0) {
    print_code_segment (op_list, IC_START, (ic + IC_START), stream);
    if (dc > 0) {
      fputc ('\n', stream);
    }
  }
  if (dc > 0) {
    print_data_segment (data_segment, (IC_START + ic), (ic + dc + IC_START),
                        stream);
  }
}
