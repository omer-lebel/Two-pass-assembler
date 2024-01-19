/*
 Created by OMER on 1/1/2024.
*/


#include "input.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
#define BOLD "\033[1m"
#define REG "\033[0m"


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

bool isAlphaNumeric(const char *str) {
  while (*str) {
    if (!isalnum(*str)) {
      return FALSE;  /* Not alphanumeric */
    }
    str++;
  }
  return TRUE;  /* All characters are alphanumeric */
}


/* todo check what is valid macro name*/
int is_valid_mcr_name(char *s)
{
  for (; *s != '\0'; s++){
    if (!isalnum(*s) && *s != '_'){
      return FALSE;
    }
  }
  return TRUE;
}



void lineTok (LineInfo *line)
{
  size_t j=0, i = 0;
  char *p = (line->postfix);

  /* Concatenate prefix and token */
  strcat(line->prefix, line->token);
  j = strlen (line->prefix);

  /* skip empty characters in postfix and write them in prefix */
  for (; isspace(*p); j++, p++){
    line->prefix[j] = *p;
  }
  NULL_TERMINATE(line->prefix,j);

  /* find next token and update postfix: */
   /*special case for a word consisting of a comma or equal sign */
  if (*p == ',' || *p == '=') {
    line->token[i] = *p;
    i++; p++;
  }
  else{ /* coping word */
    for (; *p!= '\0' && !isspace(*p) && *p != ',';
           i++, p++) {
      line->token[i] = *p;
    }
  }
  /* null terminate the token */
  NULL_TERMINATE(line->token, i);
  line->postfix = p;
}

void lineToPostfix(LineInfo *line) {
  /* concatenate prefix, token and postfix to recreate the original line */
  strcat (line->postfix, line->prefix);
  strcat (line->postfix, line->token);

  /* Reset token and postfix */
  RESET_STRING(line->prefix);
  RESET_STRING(line->token);
}



void r_msg(char* type, char*color, char* msg_before, LineInfo* line, char
*msg_after)
{
  size_t i;

  /* Print file and line number, error or warning type (fileNum:i error:) */
  printf("%s:%-2lu %s%s: " RESET, line->file, line->num, color, type);

  /* Print message context, token, and additional message */
  printf("%s" BOLD "'%s'" REG "%s\n", msg_before, line->token, msg_after);


  /* print line number and the line with the token bolded in color
   i | line with error cause bolted in color */
  printf (" %-2lu | %s%s%s" RESET "%s\n",
          line->num, line->prefix, color, line->token, line->postfix);

  /* print an arrow pointing to the location of the token in the line
       |           ^~~~~~~~~                 */
  printf(" %-2s |", " ");
  for (i = 0; i < strlen (line->prefix) ; i++){
    printf(" ");
  }
  printf(" %s^", color);
  for (i = 0; i < strlen (line->token) - 1; i++){
    printf("%s~", color);
  }
  printf(RESET "\n");
}


void r_error(char* msg_before, LineInfo* line, char *msg_after){
  r_msg("error", RED, msg_before,line, msg_after);
}

void r_warning(char* msg_before, LineInfo* line, char *msg_after){
  r_msg("warning", YEL, msg_before,line, msg_after);
}