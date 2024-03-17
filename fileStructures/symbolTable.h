/*
 Created by OMER on 1/24/2024.
*/


#ifndef _MY_SYMBOL_TABLE_H_
#define _MY_SYMBOL_TABLE_H_

#include "../setting.h"
#include "../utils/linkedList.h"

typedef enum SymbolType
{
    DATA, OPERATION, DEFINE, EXTERN
} SymbolType;

typedef enum ARE
{
    EXTERNAL = 1, RELOCATABLE = 2
} ARE;

typedef struct Symbol
{
    union
    {
        size_t address;
        int val;    /* for define */
    };
    SymbolType type;
    Bool isEntry;
    ARE are; /*todo delete */
} Symbol;

LinkedList *init_symbol_table (void);
void *init_symbol (const void *data);
void print_symbol (const char *word, const void *data, FILE *pf);
exit_code add_symbol (LinkedList *symbol_table, const char *label,
                      SymbolType type, size_t address, ARE are, int val);
void update_data_symbol_addresses (LinkedList *symbol_table, size_t IC);

#endif /* _MY_SYMBOL_TABLE_H_ */
