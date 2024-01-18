//
// Created by OMER on 1/10/2024.
//

#ifndef _PASSONE_H_
#define _PASSONE_H_

#include "../input.h"
#include "../linkedList.h"

typedef enum {
    CHAR_TYPE,
    INT_TYPE
} DS_Type;

typedef struct DS_Word{
    DS_Type type;
    int val;
} DS_Word;


typedef enum symbol_type
{
    DIRECTIVE, OPERATION, DEFINE
} symbol_type;

typedef struct symbol
{
    size_t address;
    symbol_type type;
    bool isEntry;
    bool isExtern;
} symbol;



int passOne (FILE *input);

/* *************************************************
* ...Function to handle the macro's linked list ...
***************************************************/

void *init_symbol (const void *data);
void print_symbol (const char *word, const void *data, FILE *pf);

#define INIT_SIZE 1000
#define ERROR_STR "ERROR in line %d in def of str %s "
#endif //_PASSONE_H_
