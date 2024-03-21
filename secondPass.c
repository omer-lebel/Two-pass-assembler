/*
 Created by OMER on 3/19/2024.
*/


#include "secondPass.h"

Symbol* unresolved_usages_in_operand(Operand *operand){
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

void print_usages_errors (Op_List *op_list){
  Op_Line *op_line;
  int i = 0;
  Symbol *src_symbol = NULL, *target_symbol = NULL;
  while ((op_line = get(op_list, i++))){
    /* src */
    if ((src_symbol = unresolved_usages_in_operand (&op_line->analyze->src))){
      r_error ("src error", op_line->line_part, "");
    }

    /* target */
    if ((target_symbol = unresolved_usages_in_operand (&op_line->analyze->target))
        && target_symbol != src_symbol){
      r_error ("target error", op_line->line_part, "");
    }
  }
}


void print_entry_errors(Entry_List *entry_list)
{
  Entry_line *entry_line;
  Symbol *symbol;
  Symbol_Data *symbol_data;
  int i = 0;
  while ((entry_line = get(entry_list, i++))){
    symbol = entry_line->symbol;
    symbol_data = (Symbol_Data*) symbol->data;
    if (symbol_data->type == UNRESOLVED_ENTRY
        || symbol_data->type == UNRESOLVED_ENTRY_USAGE){
      r_error ("unresolved entry declaration ", entry_line->part, "");
    }
  }
}



exit_code write_analyze(file_analyze *f){
  FILE *ob_file, *ext_file, *ent_file;

  if (f->DC > 0 || f->IC > 0){
    ob_file = open_file(f->file_name, ".ob", "w");
    if (!(ob_file)){
      return ERROR;
    }
    print_memory_img(f->op_list, f->IC, f->data_segment, f->DC, ob_file);
    fclose (ob_file);
  }

  if (f->entry_list->size > 0){
    ent_file = open_file(f->file_name, ".ent", "w");
    if (!(ent_file)){
      return ERROR;
    }
    print_entry_list (f->entry_list, ent_file);
    fclose (ent_file);
  }

  if (f->symbol_table->extern_count > 0){
    ext_file = open_file(f->file_name, ".ext", "w");
    if (!(ext_file)){
      return ERROR;
    }
    if (!print_extern_table (f->op_list, ent_file)){
      fclose (ext_file);
      return MEMORY_ERROR;
    }
    fclose (ext_file);
  }
  return SUCCESS;
}

exit_code secondPass (file_analyze *f)
{
  exit_code res = SUCCESS;
  int unresolved_entries = f->symbol_table->unresolved_entry_count;
  int unresolved_usages = f->symbol_table->unresolved_usage_count;

  if (unresolved_entries == 0 && unresolved_usages > 0){
    print_usages_errors (f->op_list);
  }
  if (unresolved_entries > 0 && unresolved_usages == 0){
    print_entry_errors (f->entry_list);
  }
  if (unresolved_entries > 0 && unresolved_usages > 0){
    print_usages_errors (f->op_list);
    print_entry_errors (f->entry_list);
  }
  if (unresolved_entries == 0 && unresolved_usages == 0){
    if (f->error){
      return SUCCESS;
    }
    else{ //no errors at all
      update_data_symbol_addresses(f->symbol_table, f->IC);
#ifdef DEBUG
      show_symbol_table (f->symbol_table, stdout);
      fprintf (stdout, "------------------ entry list ------------------\n");
      print_entry_list (f->entry_list, stdout);
      show_op_list (f->op_list, stdout);
#endif
      res = write_analyze(f);
    }
  }
  return res;
}
