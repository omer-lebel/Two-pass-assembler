#include <stdio.h>
#include <unistd.h>

#include "input.h"
#include "preAssembler.h"

int diff (char *path)
{
  fflush (stdout);
  char command[100];

  // Construct the diff command
  sprintf (command, "diff %s/output %s/res", path, path);
  int result = system (command);
  //0- identical, 256 - different, other-error
  return (result == 0 ? 0 : 1);
}

int test1_isSavedWord ()
{
  if (isSavedWord ("mov") == TRUE
      && isSavedWord ("r7") == TRUE
      && isSavedWord (".entry") == TRUE
      && isSavedWord ("dsa") == FALSE
      && isSavedWord ("omer8118") == FALSE) {
    return TRUE;
  }
  return FALSE;
}

int test2_remove_comment (char *path)
{
  char fileName[100];

  sprintf (fileName, "%s/input", path);
  FILE *input = fopen (fileName, "r");
  if (!input) {
    return FALSE;
  }
  sprintf (fileName, "%s/output", path);
  FILE *res = fopen (fileName, "w");
  if (!res) {
    return FALSE;
  }
  preAssembler (input, res);
  fclose (res);
  return (diff (path) == 0);
}

int main (int argc, char *argv[])
{
       int x    ,    y    ;
  FILE *input = fopen (argv[1], "r");
  FILE *output = fopen ("output", "w");
  if (!input || !output)
  {
    printf ("ERROR while opening file");
    return EXIT_FAILURE;
  }
  preAssembler (input, output);
  fclose (input);
  fclose (output);


/*
  int res;
  printf ("\ntest 1: isSavedWord ----- ");
  res = test1_isSavedWord ();
  printf (res ? "PASS\n" : "ERROR\n");

  printf ("\ntest 2: remove_comment --- ");
  res = test2_remove_comment("./Testing/test2_comments");
  printf (res ? "PASS\n" : "ERROR\n");*/



  return 0;
}
