/*
 Created by OMER on 1/24/2024.
*/


#ifndef _MY_SYMBOL_TABLE_H_
#define _MY_SYMBOL_TABLE_H_

#include "../setting.h"
#include "../utils/linkedList.h"

typedef enum SymbolType
{
    DATA, CODE, DEFINE, EXTERN,
    UNRESOLVED_USAGE, UNRESOLVED_ENTRY, UNRESOLVED_ENTRY_USAGE
} SymbolType;

typedef enum EntryFlag
{
    NOT_ENTRY, IS_Entry
} EntryFlag;


typedef struct Symbol_Data
{
    int val;        /* address or val of define */
    SymbolType type;
    EntryFlag isEntry;
} Symbol_Data;

typedef Node Symbol_N;


typedef struct Symbol_Table{
    LinkedList *database;
    int extern_count;
    int entry_count;
    int unresolved_entry_count;
    int unresolved_usage_count;
}Symbol_Table;




Symbol_Table *new_symbol_table (void);
void *init_symbol_data (const void *data);
Symbol_N *add_symbol (Symbol_Table *table, const char *label,
                      SymbolType type, int val, EntryFlag isEntry);
Symbol_N* find_symbol(Symbol_Table *table, const char *name);
Symbol_N *get_next_symbol(Symbol_Table *table);
void print_symbol (const char *word, const void *data, FILE *stream);
void show_symbol_table(Symbol_Table *table, FILE *stream);
void free_symbol_table (Symbol_Table *table);


void update_data_symbol_addresses (Symbol_Table *symbol_table, int IC);

#endif /* _MY_SYMBOL_TABLE_H_ */
