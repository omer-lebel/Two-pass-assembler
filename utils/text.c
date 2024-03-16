/*
 Created by OMER on 1/1/2024.
*/


#include "text.h"

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

Bool isSavedWord (const char *s)
{
  int i;
  for (i = 0; SavedWord[i][0] != '\0'; i++) {
    if (strcmp (s, SavedWord[i]) == 0) {
      return TRUE;
    }
  }
  return FALSE;
}

Bool isAlphaNumeric (const char *str)
{
  while (*str) {
    if (!isalnum(*str)) {
      return FALSE;  /* Not alphanumeric */
    }
    str++;
  }
  return TRUE;  /* All characters are alphanumeric */
}

Bool valid_identifier (LinePart *line, char *name, Bool print_err)
{
  if (!isalpha(name[0])) {
    if (print_err) {
      r_error ("", line, " starts with a non-alphabetic character");
    }
    return FALSE;
  }
  if (!isAlphaNumeric (name)) {
    if (print_err) {
      r_error ("", line, " contains non-alphanumeric characters");
    }
    return FALSE;
  }
  if (isSavedWord (name)) {
    if (print_err) {
      r_error ("", line, " is a reserved keyword that cannot be used as an "
                         "identifier");
    }
    return FALSE;
  }
  return TRUE;
}

void trim_end (char *str)
{
  int i = (int) strlen (str);
  while (isspace(str[--i])) {}
  NULL_TERMINATE(str, i + 1);
}

char *get_line (FILE *file, char *buffer, size_t buff_size, Bool *overflow)
{
  size_t i = 0;
  int c;
  *overflow = FALSE;

  while ((c = fgetc (file)) != EOF && i < buff_size - 1) {
    buffer[i++] = (char) c;
    if (c == '\n') {
      break;
    }
  }
  buffer[i] = '\0';

  /* Check if the remaining characters are all spaces */
  while (c != EOF && c != '\n') {
    if (!isspace(c)) {
      *overflow = TRUE;
    }
    c = fgetc (file);
  }

  return (i != 0) ? buffer : NULL;
}

void lineTok (LinePart *line)
{
  size_t i = 0, j = 0;
  char *p = line->postfix;

  /* Concatenate prefix and token */
  strcat (line->prefix, line->token);
  j = strlen (line->prefix);

  /* skip empty characters in postfix and write them in prefix */
  while (isspace(*p)) {
    line->prefix[j++] = *p++;
  }
  NULL_TERMINATE(line->prefix, j);

  /* find the next token: */
  /* special case for a word consisting a special sign */
  if (*p && strchr (",=]", *p)) {
    line->token[0] = *p;
    NULL_TERMINATE(line->token, 1);
    ++p;
  }
  else { /* coping word */
    while (*p != '\0' && !isspace(*p) && !strchr ("[,=]", *p)) {
      line->token[i++] = *p++;
    }
    if (*p == '['){
      line->token[i++] = *p++;
    }
    NULL_TERMINATE(line->token, i);
  }
  /* update the postfix */
/*  strcpy (line->postfix, p); */ /* todo debug why it's not working */
  for (i = 0; i <= strlen (p); ++i) {
    line->postfix[i] = p[i];
  }
}

void restartLine (LinePart *line_info)
{
  line_info->num++;
  RESET_STR(line_info->prefix);
  RESET_STR(line_info->token);
}

void copy_line_info (LinePart *dst, LinePart *src)
{
  strcpy (dst->prefix, src->prefix);
  strcpy (dst->token, src->token);
  strcpy (dst->postfix, src->postfix);
  dst->num = src->num;
}

void lineToPostfix (LinePart *line)
{
  /* concatenate prefix, token and postfix to recreate the original line */
  strcat (line->prefix, line->token);
  strcat (line->prefix, line->postfix);

  strcpy (line->postfix, line->prefix); /*move the whole line to post fix */

  /* Reset token and postfix */
  RESET_STR(line->prefix);
  RESET_STR(line->token);
}

void r_msg (char *type, char *color, char *msg_before, LinePart *line, char
*msg_after)
{
  signed i;
  /* Print file and line number, error or warning type (fileNum:i error:) */
  printf ("%s:%-2lu %s%s: " RESET, line->file, line->num, color, type);

  /* Print message context, token, and additional message */
  if (IS_EMPTY(line->token)) {
    if (IS_EMPTY(msg_before)) {
      printf ("%s\n", msg_after);
    }
    else {
      printf ("%s\n", msg_before);
    }
  }
  else {
    printf ("%s" BOLD "'%s'" REG "%s\n", msg_before, line->token, msg_after);
  }


  /* print line number and the line with the token bolded in color
   i | line with error cause bolted in color */
  printf (" %-2lu | %s%s%s" RESET "%s\n",
          line->num, line->prefix, color, line->token, line->postfix);

  /* print an arrow pointing to the location of the token in the line
       |           ^~~~~~~~~                 */
  printf (" %-2s |", " ");
  for (i = 0; i < (int) strlen (line->prefix); i++) {
    printf (" ");
  }
  printf (" %s^", color);
  for (i = 0; i < (int) strlen (line->token) - 1; i++) {
    printf ("%s~", color);
  }
  printf (RESET "\n");
}

void r_error (char *msg_before, LinePart *line, char *msg_after)
{
  r_msg ("error", RED, msg_before, line, msg_after);
}

void r_warning (char *msg_before, LinePart *line, char *msg_after)
{
  r_msg ("warning", YEL, msg_before, line, msg_after);
}
