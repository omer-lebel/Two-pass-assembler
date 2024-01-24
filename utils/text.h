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


typedef struct LineInfo{
    char* file;
    size_t num;
    char *prefix;
    char *token;
    char *postfix;
}LineInfo;

Bool isEmptyTok(char* token);
Bool isAlphaNumeric(const char *str);
void lineTok (LineInfo *line);
void lineToPostfix(LineInfo *line);
void r_error(char* msg_before, LineInfo* line, char *msg_after);
void r_warning(char* msg_before, LineInfo* line, char *msg_after);

/**
 * Extracts a token from the input string, skipping leading whitespaces.
 * A comma is treated both as a token and a delimiter.
 *
 * @param str The input string.
 * @param token The buffer to store the extracted token.
 * @return Pointer to the next char after the extracted token or NULL if
 * reach end of string.
 */
char *newToken (char *line, char *token);

int isSavedWord(const char *s);

int is_valid_mcr_name(char *s);


#endif /* _TEXT_H_ */
