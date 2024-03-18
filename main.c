/*
 Created by OMER on 1/10/2024.
*/


#include <stdio.h>
#include <stdlib.h>

#include "setting.h"
#include "preAssembler.h"
#include "firstPass.h"
#include "secondPass.h"

#define INPUT_IND 1



exit_code init_file_analyze (file_analyze *f, char *file_name,
                             FILE **src_file, FILE **am_file)
{
  memset (f, 0, sizeof (file_analyze));
  strcpy (f->file_name, file_name);
  *src_file = open_file(f->file_name, "", "r");

  if (!(*src_file)){
    return ERROR;
  }
  *am_file = open_file(f->file_name, ".am", "w+");
  if (!(*am_file)){
    return ERROR;
  }
  return SUCCESS;
}

int main (int argc, char *argv[])
{
  int res = SUCCESS, i, len;
  file_analyze file_analyze;
  FILE *src_file, *am_file;

  if (argc <= 1) {
    printf ("error: must give at least one file to process\n");
    return EXIT_FAILURE;
  }

  for (i = INPUT_IND; i < argc && res != MEMORY_ERROR ; ++i) {
    res = init_file_analyze (&file_analyze, argv[i], &src_file, &am_file);
    len = (int) strlen (argv[i]);
    /* -------------------- pre ------------------- */
    if (res == SUCCESS){
      res = preAssembler (argv[i], src_file, am_file, &file_analyze.error);
      fclose (src_file);
    }
    /* ------------------- first ----------------- */
    if (res == ERROR){
      remove_file (file_analyze.file_name, ".am");
    }
    else if (res == SUCCESS){
      rewind (am_file);
      strcat(file_analyze.file_name, ".am");
      res = firstPass (am_file, &file_analyze);
      fclose (am_file);
      NULL_TERMINATE(file_analyze.file_name, len);

      /*-------------------- second ------------------ */
      /*if (res != MEMORY_ERROR){

        res = secondPass (&file_analyze);
      }*/
    }

  }
  return (res != MEMORY_ERROR ? EXIT_SUCCESS : EXIT_FAILURE);
}

