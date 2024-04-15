/**
 * @file assembler.c
 * @brief Main file for the assembler program.
 * @author OMER LEBEL
 */

/* ------------------------------- includes ------------------------------- */
#include "setting.h"
#include "fileStructures/fileStructures.h"
#include "preAssembler.h"
#include "firstPass.h"
#include "secondPass.h"
#include "utils/errors.h"
/* -------------------------------- defines -------------------------------- */
#define INPUT_IND 1
/* ---------------------- helper function declaration ---------------------- */
Bool run_assembler (char *file_name);
void raise_end_msg (exit_code file_exit_code);
/* ------------------------------------------------------------------------- */

int main (int argc, char *argv[])
{
  int i;
  Bool flag = TRUE;

  if (argc <= 1) {
    raise_general_msg (no_input_file_err);
    flag = FALSE;
  }

  for (i = INPUT_IND; i < argc && flag; ++i) {
    flag = run_assembler (argv[i]);
  }

  return flag ? EXIT_SUCCESS : EXIT_FAILURE;
}

/**
 * @brief Runs the assembler for a given input file.
 *
 * @param file_name Name of the input file to be assembled.
 * @return TRUE if assembly process is successful, FALSE otherwise.
 */
Bool run_assembler (char *file_name)
{
  file_analyze f;
  FILE *as_file, *am_file;
  exit_code res;
  int len = (int) strlen (file_name);

  printf ("========================[ Processing | %s.as ]========================\n", file_name);

  if (!init_file_analyze (&f, file_name, &as_file, &am_file)) {
    return TRUE;
  }

  /* -------------------- pre assembler -------------------- */
  res = preAssembler (f.file_name, as_file, am_file, &f.error);
  fclose (as_file);
  rewind (am_file);

  /* --------------------- first pass --------------------- */
  if (res == SUCCESS) {
    res = firstPass (am_file, &f);
    fclose (am_file);
    f.file_name[len] = '\0';

    /* ------------------- second pass ------------------- */
    if (res != MEMORY_ERROR) {
      res = secondPass (&f);
    }
  }

  raise_end_msg (res);
  return (res != MEMORY_ERROR) ? TRUE : FALSE;
}

/**
 * @brief Raises an end message based on the exit code of the assembly process.
 *
 * @param file_exit_code Exit code of the assembly process.
 */
void raise_end_msg (exit_code file_exit_code)
{
  switch (file_exit_code) {
    case SUCCESS:
      raise_general_msg (ended_successfully);
      break;
    case ERROR:
      raise_general_msg (assembler_failed);
      break;

    case MEMORY_ERROR:
      raise_general_msg (memory_err);
      break;
  }
}