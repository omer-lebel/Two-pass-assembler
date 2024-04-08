/*
 Created by OMER on 3/6/2024.
*/

#include "analyzer.h"

/************************* line info ***************************/

void print_data (int *arr, unsigned len)
{
  unsigned int i;
  printf ("data: " "%d", arr[0]);
  for (i = 1; i < len; ++i) {
    printf (", %d", arr[i]);
  }
  printf ("\n");
}

/*void print_line_info (LineInfo *line, char *file_name)
{
  printf ("%s:%-2lu ", file_name, line->parts->num);

  switch (line->type_l) {
    case str_l:
      printf ("string: %s\n", line->info.str.content);
      break;
    case data_l:
      print_data (line->info.data.arr, line->info.data.len);
      break;
    case ext_l:
      printf ("extern: %s\n",
              ((Symbol *) line->info.ext_ent.name)->word);
      break;
    case ent_l:
      printf ("entry: %s\n",
              ((Symbol *) line->info.ext_ent.name)->word);
      break;
    case op_l:
      print_op_line(line->info.op, stdout);
      break;
    case def_l:
      printf ("define: %s = %d\n",
              ((Symbol *) line->info.define.name)->word,
              line->info.define.val);
      break;
  }
}*/
