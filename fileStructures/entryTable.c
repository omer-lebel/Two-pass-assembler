/*
 Created by OMER on 3/6/2024.
*/


#include "entryTable.h"

void *init_entry_table (void)
{
  return create_vector (sizeof (EntrySyb));
}

Bool is_new_entry_symbol (vector *entry_table, char *new_ent)
{
  size_t i;
  EntrySyb *tmp;
  for (i = 0; i < entry_table->size; ++i) {
    tmp = (EntrySyb *) get (entry_table, i);
    if (strcmp (new_ent, tmp->name) == 0) {
      return FALSE;
    }
  }
  return TRUE;
}

EntrySyb *add_to_entry_table (vector *entry_table, char *name,
                              LinePart *line, size_t address)
{
  EntrySyb entry_syb;
  strcpy (entry_syb.name, name);
  entry_syb.address = address;
  /* coping line info */
  entry_syb.line_info = malloc (sizeof (LinePart));
  if (!entry_syb.line_info) {
    return NULL;
  }
  copy_line_part (entry_syb.line_info, line);
  return push (entry_table, &entry_syb);;
}

void print_entry_table (vector *entry_table, FILE *stream)
{
  size_t i;
  EntrySyb *entry_syb;
  for (i = 0; i < entry_table->size; ++i) {
    entry_syb = (EntrySyb *) get (entry_table, i);
    fprintf (stream, "%s\t%04u", entry_syb->name, entry_syb->address);
    if (i < entry_table->size - 1) {
      fputc ('\n', stream);
    }
  }
}

void free_entry_table (vector *entry_table)
{
  size_t i;
  EntrySyb *entry_syb;
  for (i = 0; i < entry_table->size; ++i) {
    entry_syb = (EntrySyb *) get (entry_table, i);
    free (entry_syb->line_info);
  }
  free_vector (entry_table);
}

