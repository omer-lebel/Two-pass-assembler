/*
 Created by OMER on 1/1/2024.
*/


#include "input.h"
#include <ctype.h>
#include <string.h>

/*
char* registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
char* direction[] = {".data", ".string", ".entry", ".extern", ".define"};
char* cmd_type1[] = {"mov", "cmp", "add", "sub", "lea"};
char* cmd_type2[] = {"not", "clr", "inc", "dec", "jmp", "bne", "red", "prn","jsr"};
char* cmd_type3[] = {"rts", "hlt"};
 */

char *SavedWord[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
                     ".data", ".string", ".entry", ".extern", ".define",
                     "mov", "cmp", "add", "sub", "lea",
                     "not", "clr", "inc", "dec", "jmp", "bne", "red", "prn",
                     "jsr", "rts", "hlt"};

int isSavedWord (const char *s)
{
  int i = 0;
  int len = sizeof(SavedWord) / sizeof(*SavedWord);
  for (i = 0; i < len; i++) {
    if (strcmp (s, SavedWord[i]) == 0)
    {
      return TRUE;
    }
  }
  return FALSE;
}

int isValidName(char *s)
{
  for (; *s != '\0'; s++){
    if (!isalnum(*s) && *s != '_'){
      return FALSE;
    }
  }
  return TRUE;
}

char *getToken (char *line, char *token)
{
  int i = 0;

  /* skip empty chars */
  while (isspace(*line)) { line++; }

  /* special case for a word consisting of a comma */
  if (*line == ',') {
    token[i] = ',';
    i++;
    line++;
  }
  else { /* coping word */
    for (; *line && !isspace(*line) && *line != ','; i++, line++) {
      token[i] = *line;
    }
  }
  /* null terminate */
  token[i] = '\0';

  if (i == 0) { /* empty word */
    return NULL;
  }
  return line;
}