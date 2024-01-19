/*
 Created by OMER on 1/10/2024.
*/


#include <stdio.h>
#include <stdlib.h>
#include "../../preAssembler.h"


#define INPUT_IND 1

int call_pre(char* file_name){
  FILE *src_file, *am_file;
  int res;

  /* opening src file */
  src_file = fopen (file_name, "r");
  if (!src_file) {
    printf ("error while opening '%s'\n", file_name);
    return EXIT_FAILURE;
  }

  /* opening file.am for writing */
  strcat (file_name, ".am");
  am_file = fopen (file_name, "w");
  if (!am_file) {
    printf ("error while opening %s file\n", file_name);
    fclose (src_file);
    return EXIT_FAILURE;
  }

  /* running preAssembler */
  res = preAssembler (src_file, am_file);
  fclose (src_file);
  fclose (am_file);

  return res;
}


int main (int argc, char *argv[])
{
  char fileName[100];
  int res, i;

  if (argc <= 1) {
    printf ("must give at least one file to process\n");
    return EXIT_FAILURE;
  }

  for (i = INPUT_IND; i<argc; i++){
    strcpy (fileName,argv[i]);
    /* -------------------- pre ------------------ */
    res = call_pre (fileName);
    if (res != EXIT_SUCCESS) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

