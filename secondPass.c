/*
 Created by OMER on 3/19/2024.
*/


#include "secondPass.h"

Symbol_N* unresolved_usages_in_operand(Operand *operand){
  Symbol_N *symbol;
  Symbol_Data *symbol_data;
  Addressing_Mode addr_mode = operand->add_mode;

  if (addr_mode == DIRECT_ADD || addr_mode == INDEX_ADD) {
    symbol = (Symbol_N *) operand->info.symInx.symbol;
    symbol_data = (Symbol_Data *) symbol->data;

    if (symbol_data->type == UNRESOLVED_ENTRY_USAGE
        || symbol_data->type == UNRESOLVED_USAGE) {
      return symbol;
    }
  }
  return NULL;
}

void print_usages_errors (Op_List *op_list){
  LineInfo *line;
  Symbol_N *src_symbol = NULL, *target_symbol = NULL;
  while ((line = get_next_op_line(op_list))){
    /* src */
    if ((src_symbol = unresolved_usages_in_operand (&line->info.op->src))){
      r_error ("src error", line->parts, "");
    }

    /* target */
    if ((target_symbol = unresolved_usages_in_operand (&line->info.op->target))
        && target_symbol != src_symbol){
      r_error ("src target", line->parts, "");
    }
  }
}

///* todo change */
//void print_entry_errors(Symbol_Table *symbol_table)
//{
//  Symbol_Data *symbol_data;
//  Symbol_N *symbol = symbol_table->database->head;
//  while (symbol){
//    symbol_data = (Symbol_Data*) symbol->data;
//    if (symbol_data->type == UNRESOLVED_ENTRY
//        || symbol_data->type == UNRESOLVED_ENTRY_USAGE){
//      r_error ("error in entry", NULL, "");
//    }
//    symbol = symbol->next;
//  }
//}

//void print_entry_table (Symbol_Table *symbol_table, FILE *stream)
//{
//  Symbol_Data *symbol_data;
//  Symbol_N *symbol = symbol_table->database->head;
//  int i = symbol_table->entry_count - 1;
//  while (symbol){
//    symbol_data = (Symbol_Data*) symbol->data;
//    if (symbol_data->isEntry){
//      r_error ("error in entry", NULL, "");
//      fprintf (stream, "%s\t%04u", symbol->word, symbol_data->val);
//      if (i-- > 0) {
//        fputc ('\n', stream);
//      }
//    }
//    symbol = symbol->next;
//  }
//}


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

  if (f->symbol_table->entry_count > 0){
    ent_file = open_file(f->file_name, ".ent", "w");
    if (!(ent_file)){
      return ERROR;
    }
//    print_entry_table(f->symbol_table, ent_file);
    fclose (ent_file);
  }

  if (f->symbol_table->extern_count > 0){
    ext_file = open_file(f->file_name, ".ext", "w");
    if (!(ext_file)){
      return ERROR;
    }
//    process_extern_table ();
    fclose (ext_file);
  }

  return SUCCESS;
}

exit_code secondPass (file_analyze *f)
{
  int unresolved_entries = f->symbol_table->unresolved_entry_count;
  int unresolved_usages = f->symbol_table->unresolved_usage_count;

  if (unresolved_entries == 0 && unresolved_usages > 0){
    print_usages_errors (f->op_list);
  }
  if (unresolved_entries > 0 && unresolved_usages == 0){
//    print_entry_errors (f->symbol_table);
  }
  if (unresolved_entries > 0 && unresolved_usages > 0){
    print_usages_errors (f->op_list);
//    print_entry_errors (f->symbol_table);
  }
  if (unresolved_entries == 0 && unresolved_usages == 0){
    if (f->error){
      return SUCCESS;
    }
    else{ //no errors at all
      update_data_symbol_addresses(f->symbol_table, f->IC);
      // write ob file
      // write extern file
      // write entry file
    }
  }

}
