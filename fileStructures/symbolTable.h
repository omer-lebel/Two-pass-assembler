/*
 Created by OMER on 1/24/2024.
*/


#ifndef _MY_SYMBOL_TABLE_H_
#define _MY_SYMBOL_TABLE_H_

#include "../setting.h"
#include "../utils/linkedList.h"

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

//extern LinkedList *symbols_table;

LinkedList* init_symbol_table(void);
void *init_symbol (const void *data);
void print_symbol (const char *word, const void *data, FILE *pf);
exit_code add_symbol (LinkedList *symbol_table, const char *label, SymbolType
type, size_t
address, int
isExtern, int val);


#endif /* _MY_SYMBOL_TABLE_H_ */
