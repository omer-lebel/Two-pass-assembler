/*
 Created by OMER on 1/1/2024.
*/

#ifndef _INPUT_H_
#define _INPUT_H_

#define MAX_LINE_SIZE (80+1)
#define MAX_TOKEN_SIZE (80+1)
#define DELIM " \t\r\n"

#define SAVED_WORD_NUM (8 + 5 + 16)
/* 8- register, 5-direction, 16-instruction */
enum
{
    FALSE = 0, TRUE
};


char *getToken (char *line, char *token);

int isSavedWord(const char *s);

int isValidName(char *s);


#endif /* _INPUT_H_ */
