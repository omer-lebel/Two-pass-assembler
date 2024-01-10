/*
 Created by OMER on 1/10/2024.
*/


#include <stdio.h>
#include "preAssembler.h"

int main (int argc, char *argv[])
{
  FILE *txt_file, *as_file;
  char pre_output[100];
  int res;

  if (argc <= 1){
    printf ("must give at least one file to process\n");
    return EXIT_FAILURE;
  }

  txt_file = fopen (argv[1], "r");
  if (!txt_file) {
    printf ("error while opening input file\n");
    return EXIT_FAILURE;
  }

  strcpy (pre_output, argv[1]);
  strcat (pre_output, ".as");
  as_file = fopen (pre_output, "w");
  if (!as_file) {
    printf ("error while opening %s.as file\n",argv[1]);
    return EXIT_FAILURE;
  }

  res = preAssembler (txt_file, as_file);
  fclose (txt_file);
  fclose (as_file);

  return res;
}