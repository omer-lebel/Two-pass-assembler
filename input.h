/*
 Created by OMER on 1/1/2024.
*/

#ifndef _INPUT_H_
#define _INPUT_H_

#define MAX_LINE_SIZE (80+1)
#define MAX_TOKEN_SIZE (80+1)
#define DELIM " \t\r\n"

#include <stdlib.h>

#define SAVED_WORD_NUM (8 + 5 + 16)
/* 8- register, 5-direction, 16-instruction */
typedef enum bool
{
    FALSE = 0, TRUE = 1
}bool;


typedef struct Line{
    char *prefix;
    char *token;
    char **postfix;
}Line;

void lineTok (Line *line);
void r_error(Line* line, size_t line_num, char *msg, int type);


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

int isValidName(char *s);


#endif /* _INPUT_H_ */
