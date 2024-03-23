/*
 Created by OMER on 1/10/2024.
*/


#include <stdio.h>
#include <stdlib.h>

#include "setting.h"
#include "fileStructures/fileStructures.h"
#include "preAssembler.h"
#include "firstPass.h"
#include "secondPass.h"

#define INPUT_IND 1

#define MAG   "\x1B[35m"
#define BLU   "\x1B[34m"
#define GRN   "\x1B[32m"
#define CYN   "\x1B[36m"  /*error color */
#define WHT   "\x1B[37m"

exit_code init_file_analyze (file_analyze *f, char *file_name,
                             FILE **src_file, FILE **am_file)
{
  memset (f, 0, sizeof (file_analyze));
  strcpy (f->file_name, file_name);
  *src_file = open_file(f->file_name, ".as", "r");

  if (!(*src_file)){
    return ERROR;
  }
  *am_file = open_file(f->file_name, ".am", "w+");
  if (!(*am_file)){
    return ERROR;
  }
  strcat(f->file_name, ".as");
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
    printf ( "========================[ Processing | %s.as "
             "]========================\n" ,
             argv[i]);
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
      printf (RED_COLOR "*** Preprocessor failed.\n" RESET);
    }
    else if (res == SUCCESS){
      printf( GRN "output files created:\n" RESET " - %s.am \n", argv[i]);
      rewind (am_file);
      file_analyze.file_name[len + 2] = 'm';
      res = firstPass (am_file, &file_analyze);
      fclose (am_file);

      if (res == ERROR){
        printf (RED_COLOR "*** Failed\n" RESET );
      }
      /*-------------------- second ------------------ */
      else if (res != MEMORY_ERROR){
        file_analyze.file_name[len] = '\0';
        res = secondPass (&file_analyze);
      }
    }
  }
  if (res == MEMORY_ERROR){
    printf ( RED_COLOR "*** Memory allocation failure. program stopped.\n"
    RESET);
    return EXIT_FAILURE;
  }
  printf
      ("======================================================================\n");
  return EXIT_SUCCESS;
}

