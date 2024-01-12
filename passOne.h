//
// Created by OMER on 1/10/2024.
//

#ifndef _PASSONE_H_
#define _PASSONE_H_

#include <stdlib.h>
#include "input.h"
#include "linkedList.h"

int passOne (FILE *input);

/* *************************************************
* ...Function to handle the macro's linked list ...
***************************************************/

void *init_symbol (const void *data);
void print_symbol (const char *word, const void *data, FILE *pf);

#define INIT_SIZE 1000
#define ERROR_STR "ERROR in line %d in def of str %s "
#endif //_PASSONE_H_
