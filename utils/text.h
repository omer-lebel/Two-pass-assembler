/*
 Created by OMER on 1/1/2024.
*/

#ifndef _TEXT_H_
#define _TEXT_H_

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "../setting.h"


#define IS_EMPTY(s) (*s == '\0')
#define RESET_STR(str) ((str)[0] = '\0')
#define NULL_TERMINATE(str, index) ((str)[index] = '\0')
#define REMOVE_LAST_CHAR(str) ((str)[strlen(str) - 1] = '\0')
#define IS_LABEL(str) (str[strlen (str) - 1] == ':')


typedef struct LinePart{
    char* file;
    char line[MAX_LINE_LEN];
    char *prefix;
    char *token;
    char *postfix;
    size_t num;
}LineParts;

void trim_end(char *str);
Bool isSavedWord(const char *s);
Bool isAlphaNumeric(const char *str);



char* get_line(FILE *file, char *buffer, size_t buff_size, Bool *overflow);
void lineTok (LineParts *line);
void init_line_parts (LineParts *line_part, char *prefix_buf,
                      char *token_buf, char *postfix_buf);
void restart_line_parts (LineParts *line_part);
void lineToPostfix(LineParts *line);
void get_identifier_tok (LineParts *line, Bool has_label);
void move_one_char_to_prefix(LineParts *line);
void splitLine(LineParts *parts, char *wanted_tok);


#endif /* _TEXT_H_ */
