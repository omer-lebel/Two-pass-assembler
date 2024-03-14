/*
 Created by OMER on 3/6/2024.
*/


#include "externTable.h"

vector *init_extern_table (void)
{
  return create_vector (sizeof (ExternSyb));
}

ExternSyb *add_to_extern_table (vector *extern_table, char *name,
                                size_t address)
{
  ExternSyb extern_syb;
  strcpy (extern_syb.name, name);
  extern_syb.location = create_vector (sizeof (size_t));
  if (!extern_syb.location) {
    return NULL;
  }
  if (!push (extern_syb.location, &address)) {
    return NULL;
  }
  return push (extern_table, &extern_syb);
}

size_t *add_location (ExternSyb *extern_syb, size_t address)
{
  return push (extern_syb->location, &address);
}

ExternSyb *find_extern_syb (vector *extern_table, char *name)
{
  size_t i;
  ExternSyb *tmp;
  for (i = 0; i < extern_table->size; ++i) {
    tmp = (ExternSyb *) get (extern_table, i);
    if (strcmp (name, tmp->name) == 0) {
      return tmp;
    }
  }
  return NULL;
}

void print_extern_table (vector *extern_table, FILE *stream)
{
  size_t i, j;
  ExternSyb *ext;
  size_t *line_num;
  for (i = 0; i < extern_table->size; ++i) {
    ext = (ExternSyb *) get (extern_table, i);
    for (j = 0; j < ext->location->size; ++j) {
      line_num = (size_t *) get (ext->location, j);
      fprintf (stream, "%s\t%04lu", ext->name, *line_num);
      if ((i < extern_table->size - 1) ||
          (i == extern_table->size - 1 && (j < ext->location->size - 1))) {
        fputc ('\n', stream);
      }
    }
  }
}

void free_extern_table (vector *extern_table)
{
  size_t i;
  ExternSyb *tmp;
  for (i = 0; i < extern_table->size; ++i) {
    tmp = (ExternSyb *) get (extern_table, i);
    free_vector (tmp->location);
  }
  free_vector (extern_table);
}

