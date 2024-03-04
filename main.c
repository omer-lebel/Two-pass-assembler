//
// Created by OMER on 1/10/2024.
//

#include <stdio.h>
#include <stdlib.h>

#include "setting.h"
#include "preAssembler.h"
#include "firstPass.h"



#define INPUT_IND 1



void init_new_file(file_analyze *file){
  memset(file, 0, sizeof(file_analyze));
}

int call_pre (file_analyze *file)
{
  FILE *src_file, *am_file;
  int res;

  /* opening src file */
  src_file = fopen (file->file_name, "r");
  if (!src_file) {
    printf ("error while opening '%s'\n", file->file_name);
    return EXIT_FAILURE;
  }

  /* opening file.am for writing */
  strcat (file->file_name, ".am");
  am_file = fopen (file->file_name, "w");
  if (!am_file) {
    printf ("error while opening %s file\n", file->file_name);
    fclose (src_file);
    return EXIT_FAILURE;
  }

  //running preAssembler
  res = preAssembler (src_file, am_file);
  fclose (src_file);
  fclose (am_file);

  return res;
}

int call_first (file_analyze *file)
{
  FILE *am_file;
  int res;

  /* opening file.am for reading */
  am_file = fopen (file->file_name, "r");
  if (!am_file) {
    printf ("error while opening %s file\n", file->file_name);
    return EXIT_FAILURE;
  }

  /* running first pass */
  res = firstPass (am_file, file);
  fclose (am_file);

  return res;
}

int main (int argc, char *argv[])
{
  char fileName[100];
  int res, i;
  exit_code no_error = SUCCESS;
  file_analyze file_analyze;

  init_assembler_setting ();

  if (argc <= 1) {
    printf ("must give at least one file to process\n");
    return EXIT_FAILURE;
  }

  for (i = INPUT_IND; i<argc; i++) {
    strcpy(file_analyze.file_name, argv[i]);
    strcpy (fileName, argv[i]);
    /* -------------------- pre ------------------- */
    res = call_pre (&file_analyze);
    if (res != EXIT_SUCCESS) {
      return EXIT_FAILURE;
    }

    /* ------------------- first ----------------- */
    res = call_first (&file_analyze);
    if (res != EXIT_SUCCESS) {
      return EXIT_FAILURE;
    }

    /* -------------------- second ------------------ */
    /*res = call_second (fileName, &no_error);
    if (res != EXIT_SUCCESS) {
      return EXIT_FAILURE;
    }*/
  }
  return EXIT_SUCCESS;
}

