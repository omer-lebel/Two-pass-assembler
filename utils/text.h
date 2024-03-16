/*
 Created by OMER on 1/1/2024.
*/

#ifndef _TEXT_H_
#define _TEXT_H_

/*#define DELIM " \t\r\n"*/

#define IS_EMPTY(s) (*s == '\0')
#define RESET_STR(str) ((str)[0] = '\0')
#define NULL_TERMINATE(str, index) ((str)[index] = '\0')

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "../setting.h"

#define SAVED_WORD_NUM (8 + 5 + 16)
/* 8- register, 5-direction, 16-instruction */


typedef struct LinePart{
    char* file; /* todo delete!! */
    char prefix[MAX_LINE_LENGTH];
    char token[MAX_LINE_LENGTH];
    char postfix[MAX_LINE_LENGTH];
    size_t num;
}LinePart;

Bool isLabel (const char *str);
Bool isSavedWord(const char *s);
Bool isAlphaNumeric(const char *str);
Bool valid_identifier (LinePart *line, char *name, Bool print_err);
void trim_end(char *str);

char* get_line(FILE *file, char *buffer, size_t buff_size, Bool *overflow);
void lineTok (LinePart *line);
void restartLine (LinePart *line);
void copy_line_info(LinePart *dst, LinePart *src);
void lineToPostfix(LinePart *line);

void r_error(char* msg_before, LinePart* line, char *msg_after);
void r_warning(char* msg_before, LinePart* line, char *msg_after);


#endif /* _TEXT_H_ */
