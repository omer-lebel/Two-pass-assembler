/*
 Created by OMER on 1/1/2024.
*/


#include "input.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


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


void lineTok (Line *line)
{
  size_t j, i = 0;
  char *p = *(line->postfix);

  // Concatenate prefix and token
  strcat(line->prefix, line->token);
  j = strlen (line->prefix);

  // skip empty characters in postfix and write them in prefix
  for (; isspace(*p); j++, p++){
    line->prefix[j] = *p;
  }
  line->prefix[j] = '\0';

  // find next token and update postfix:
  /* special case for a word consisting of a comma */
  if (*p == ',') {
    line->token[i] = ',';
    i++; p++;
  }
  else{ /* coping word */
    for (; *p!= '\0' && !isspace(*p) && *p != ',';
           i++, p++) {
      line->token[i] = *p;
    }
  }
  /* null terminate the token */
  line->token[i] = '\0';
  *line->postfix = p;
}


void r_error(Line* line, size_t line_num, char *msg, int type){
  printf("file:%-2lu " RED "error: ",line_num);
  printf(RESET"'"  YEL"%s" RESET"' %s\n", line->token, msg);

  printf ("\t%-2lu | %s" RED "%s" RESET "%s\n",line_num, line->prefix,
          line->token, *line->postfix);
}