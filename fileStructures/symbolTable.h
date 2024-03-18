/*
 Created by OMER on 1/24/2024.
*/


#ifndef _MY_SYMBOL_TABLE_H_
#define _MY_SYMBOL_TABLE_H_

#include "../setting.h"
#include "../utils/linkedList.h"

typedef enum SymbolType
{
    DATA, CODE, DEFINE, EXTERN, UNRESOLVED
} SymbolType;

typedef struct Symbol
{
    int val;    /* address or val of define */
    SymbolType type;
    Bool isEntry;
    Bool isResolved;
} Symbol;

LinkedList *init_symbol_table (void);
void *init_symbol (const void *data);
void print_symbol (const char *word, const void *data, FILE *pf);
Node* add_symbol (LinkedList *symbol_table, const char *label,
                      SymbolType type, int val, Bool isEntry);
void update_data_symbol_addresses (LinkedList *symbol_table, int IC);

#endif /* _MY_SYMBOL_TABLE_H_ */
