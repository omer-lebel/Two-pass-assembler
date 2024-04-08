#include "text.h"

void trim_end (char *str)
{
  int i = (int) strlen (str);
  if (!IS_EMPTY(str)){
    while (isspace(str[--i])) {}
    NULL_TERMINATE(str, i + 1);
  }
}

Bool is_saved_word (const char *s)
{
  int i;
  for (i = 0; SavedWord[i][0] != '\0'; i++) {
    if (strcmp (s, SavedWord[i]) == 0) {
      return TRUE;
    }
  }
  return FALSE;
}

Bool is_alpha_numeric (const char *str)
{
  while (*str) {
    if (!isalnum(*str)) {
      return FALSE;  /* Not alphanumeric */
    }
    str++;
  }
  return TRUE;  /* All characters are alphanumeric */
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

void lineTok (LineParts *line)
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
    if (*p == '[') {
      line->token[i++] = *p++;
    }
    NULL_TERMINATE(line->token, i);
  }

  for (i = 0; i <= strlen (p); ++i) {
    line->postfix[i] = p[i];
  }
}
void init_line_parts (LineParts *line_part, char *prefix_buffer,
                      char *token_buffer, char *postfix_buffer)
{
  line_part->prefix = prefix_buffer;
  line_part->token = token_buffer;
  line_part->postfix = postfix_buffer;
  line_part->num = 0;
}

void restart_line_parts (LineParts *line_part)
{
  RESET_STR(line_part->prefix);
  RESET_STR(line_part->token);
  strcpy (line_part->postfix, line_part->line);
  line_part->num++;
}

void get_identifier_tok (LineParts *line, Bool has_label)
{
  line_to_postfix (line);
  if (has_label) {
    lineTok (line); /*move to first word */
  }
  lineTok (line); /* move to directive (.extern / .entry / .define)*/
  lineTok (line); /* move to identifier  */
}

void line_to_postfix (LineParts *line)
{
  /* concatenate prefix, token and postfix to recreate the original line */
  strcat (line->prefix, line->token);
  strcat (line->prefix, line->postfix);

  strcpy (line->postfix, line->prefix); /*move the whole line to post fix */

  /* Reset token and postfix */
  RESET_STR(line->prefix);
  RESET_STR(line->token);
}

void move_one_char_to_prefix(LineParts *line){
  int prefix_len = (int) strlen (line->prefix);
  line->prefix[prefix_len] = line->token[0];
  NULL_TERMINATE(line->prefix, prefix_len + 1);
  line->token++;
}

void split_line(LineParts *parts, char *wanted_tok) {

  // Find the position of the token within the line
  char *tokenPos = strstr(parts->line, wanted_tok);

  // Copy prefix
  strncpy(parts->prefix, parts->line, tokenPos - parts->line);
  parts->prefix[tokenPos - parts->line] = '\0';

  // Copy token
  strcpy(parts->token, wanted_tok);

  // Copy postfix
  strcpy(parts->postfix, tokenPos + strlen(wanted_tok));
}