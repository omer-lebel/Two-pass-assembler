/*
 Created by OMER on 3/19/2024.
*/


#include "secondPass.h"

#include "utils/errors.h"
#include "utils/vector.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/codeSeg.h"
#include "fileStructures/dataSeg.h"
#include "fileStructures/entryLines.h"
#include "fileStructures/externUsages.h"

void print_memory_img (Op_List *op_list, int ic, Data_Segment *data_segment,
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

Symbol *unresolved_usages_in_operand (Operand *operand)
{
  Symbol *symbol;
  Symbol_Data *symbol_data;
  Addressing_Mode addr_mode = operand->add_mode;

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

void print_usages_errors (Op_List *op_list, char *file_name)
{
  Op_Line *op_line;
  int i = 0;
  Symbol *src_symbol = NULL, *target_symbol = NULL;
  while ((op_line = get (op_list, i++))) {
    /* src */
    if ((src_symbol = unresolved_usages_in_operand (&op_line->analyze->src))) {
      raise_second_pass_err (op_line->parts, src_symbol->word, file_name);
    }

    /* target*/
    /* print error only if target makes new error for the line  */
    if ((target_symbol = unresolved_usages_in_operand (&op_line->analyze->target))) {
      if (target_symbol != src_symbol && !src_symbol) {
        raise_second_pass_err (op_line->parts, target_symbol->word, file_name);
      }
    }
  }
}

void print_entry_errors (Entry_List *entry_list, char *file_name)
{
  Entry_line *entry_line;
  Symbol *symbol;
  Symbol_Data *symbol_data;
  int i = 0;
  while ((entry_line = get (entry_list, i++))) {
    symbol = entry_line->symbol;
    symbol_data = (Symbol_Data *) symbol->data;
    if (symbol_data->type == UNRESOLVED_ENTRY
        || symbol_data->type == UNRESOLVED_ENTRY_USAGE) {
      raise_second_pass_err (entry_line->parts, entry_line->symbol->word, file_name);
    }
  }
}

exit_code print_analyze (file_analyze *f)
{
  FILE *ob_file, *ent_file, *ext_file;
  exit_code res;

  if (f->DC > 0 || f->IC > 0) {
    ob_file = open_file (f->file_name, ".ob", "w");
    if (!(ob_file)) {
      return ERROR;
    }
    print_memory_img (f->op_list, f->IC, f->data_segment, f->DC, ob_file);
    printf (" -- %s.ob created \n", f->file_name);
    fclose (ob_file);
  }

  if (f->entry_list->size > 0) {
    ent_file = open_file (f->file_name, ".ent", "w");
    if (!(ent_file)) {
      return ERROR;
    }
    print_entry_list (f->entry_list, ent_file);
    printf (" -- %s.ent created \n", f->file_name);
    fclose (ent_file);
  }

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

exit_code secondPass (file_analyze *f)
{
  exit_code res = ERROR;
  int unresolved_entries = f->symbol_table->unresolved_entry_count;
  int unresolved_usages = f->symbol_table->unresolved_usage_count;

  int len = (int) strlen (f->file_name);
  strcat (f->file_name, ".am");

  if (unresolved_entries == 0 && unresolved_usages > 0) {
    print_usages_errors (f->op_list, f->file_name);
  }
  if (unresolved_entries > 0 && unresolved_usages == 0) {
    print_entry_errors (f->entry_list, f->file_name);
  }
  if (unresolved_entries > 0 && unresolved_usages > 0) {
    print_usages_errors (f->op_list, f->file_name);
    print_entry_errors (f->entry_list, f->file_name);
  }

  f->file_name[len] = '\0';
  if (unresolved_entries == 0 && unresolved_usages == 0) {
    if (f->error == 0) {
      update_data_symbol_addresses (f->symbol_table, f->IC);
      res = print_analyze (f);
    }
  }

#ifdef DEBUG
  display_symbol_table (f->symbol_table, stdout);
  fprintf (stdout, "\n------------------ entry list ------------------\n");
  print_entry_list (f->entry_list, stdout);
  display_op_list (f->op_list, stdout);
  display_data_segment (f->data_segment, stdout);
#endif

  free_file_analyze (f);
  return res;
}
