//
// Created by OMER on 3/20/2024.
//

#ifndef _EXTERN_USAGES_H_
#define _EXTERN_USAGES_H_

#include "../setting.h"
#include "symbolTable.h"
#include "../utils/vector.h"

typedef vector Extern_Table;

typedef struct Extern_Syb{
    Symbol_N *symbol;
    vector *usages;
}Extern_Syb;

Extern_Table * new_extern_table(int size);

Extern_Syb* add_to_extern_table(Extern_Table *extern_table, char* name,
                                int address);

size_t* add_location(Extern_Syb *extern_syb, int address);

Extern_Syb* find_extern_syb(vector* extern_table, char* name);

void print_extern_table (vector *extern_table, FILE *file);

void free_extern_table(vector* extern_table);

#endif //_EXTERN_USAGES_H_
