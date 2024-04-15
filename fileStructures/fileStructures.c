#include "fileStructures.h"

Bool init_file_analyze (file_analyze *f, char *file_name,
                        FILE **as_file, FILE **am_file)
{
  memset (f, 0, sizeof (file_analyze));
  strcpy (f->file_name, file_name);
  *as_file = open_file(f->file_name, ".as", "r");

  if (!(*as_file)){
    return FALSE;
  }
  *am_file = open_file(f->file_name, ".am", "w+");
  if (!(*am_file)){
    fclose (*as_file);
    return FALSE;
  }
  strcat(f->file_name, ".as");
  return TRUE;
}

void display_debug(file_analyze *f, FILE *stream, char *pass){
  fprintf (stream, "\n======================================================\n"
                   "                    %s pass                      \n"
                   "======================================================\n"
                   "", pass);
  display_symbol_table (f->symbol_table, stream);
  fprintf (stream, "\n------------------ entry list ------------------\n");
  print_entry_list (f->entry_list, stream);
  display_op_lines_list (f->op_list, stream);
  display_data_segment (f->data_segment, stream);
}

void free_file_analyze (file_analyze *f)
{
  free_symbol_table (f->symbol_table);
  free_op_lines_list (f->op_list);
  free_data_segment (f->data_segment);
  free_entry_lines_list (f->entry_list);
  memset (f, 0, sizeof (file_analyze));
}

FILE *open_file (char *file_name, const char *extension, const char *mode)
{
  FILE *tmp;
  size_t len = strlen (file_name);
  strcat (file_name, extension);
  tmp = fopen (file_name, mode);
  if (!tmp) {
    raise_open_file_error (file_name);
    return NULL;
  }
  else {
    file_name[len] = '\0'; /* remove extension */
    return tmp;
  }
}

void remove_file (char *file_name, const char *extension)
{
  size_t len = strlen (file_name);
  strcat (file_name, extension);
  remove (file_name);
  file_name[len] = '\0'; /* remove extension */
}
