//
// Created by OMER on 3/6/2024.
//

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
  int i;
  ExternSyb *tmp;
  for (i = 0; i < extern_table->size; ++i) {
    tmp = (ExternSyb *) get (extern_table, i);
    if (strcmp (name, tmp->name) == 0) {
      return tmp;
    }
  }
  return NULL;
}

void print_extern_table (vector *extern_table, char *file_name)
{
  int i, j;
  ExternSyb *ext;
  size_t *line_num;
  printf ("\n----------------- extern table -----------------\n");
  for (i = 0; i < extern_table->size; ++i) {
    ext = (ExternSyb *) get (extern_table, i);
    for (j = 0; j < ext->location->size; ++j){
      line_num = (size_t*) get (ext->location, j);
      printf ("%s\t %04lu\n", ext->name, *line_num);
    }
  }
}

void free_extern_table (vector *extern_table)
{
  int i;
  ExternSyb *tmp;
  for (i = 0; i < extern_table->size; ++i) {
    tmp = (ExternSyb *) get (extern_table, i);
    free_vector (tmp->location);
  }
  free_vector (extern_table);
}

