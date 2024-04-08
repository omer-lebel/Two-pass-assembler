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

void free_file_analyze (file_analyze *f)
{
  free_symbol_table (f->symbol_table);
  free_op_list (f->op_list);
  free_data_segment (f->data_segment);
  free_entry_list (f->entry_list);
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
