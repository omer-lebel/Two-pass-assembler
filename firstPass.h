/*
 Created by OMER on 1/15/2024.
*/


#ifndef _FIRSTPASS_H_
#define _FIRSTPASS_H_

#include "textUtils.h"
#include "linkedList.h"

#define MAX_INT 8191
#define MIN_INT (-8191)

/****************** symbols *******************/

typedef enum SymbolType
{
    DIRECTIVE, OPERATION, DEFINE
} SymbolType;

typedef enum IsExtern
{
    EXTERNAL, NOT_EXTERNAL
} IsExtern;

typedef struct Symbol
{
    size_t address;
    SymbolType type;
    Bool isEntry;
    IsExtern isExtern;
    int val;
} Symbol;


/****************** data segment *******************/

typedef enum DsType {
    CHAR_TYPE,
    INT_TYPE
} DsType;

typedef struct DsWord{
    DsType type;
    int val;
} DsWord;


/****************** text segment *******************/





int firstPass(FILE *input_file, char* file_name, exit_code *no_error);
/* *************************************************
* ...Function to handle the macro's linked list ...
***************************************************/

void *init_symbol (const void *data);
void print_symbol (const char *word, const void *data, FILE *pf);


#endif /*_FIRSTPASS_H_ */
