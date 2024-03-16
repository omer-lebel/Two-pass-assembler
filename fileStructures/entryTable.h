/*
 Created by OMER on 3/6/2024.
*/


#ifndef _ENTRY_LIST_H_
#define _ENTRY_LIST_H_

#include "../setting.h"
#include "../utils/text.h"
#include "../utils/vector.h"

typedef struct EntrySyb
{
    char name[MAX_LINE_LENGTH];
    LinePart *line_info;
    unsigned int address;
} EntrySyb;

void* init_entry_table(void);

Bool is_new_entry_symbol(vector* entry_table, char *new_ent);

EntrySyb* add_to_entry_table(vector* entry_table, char *name, LinePart
*line, size_t address);

void print_entry_table(vector* entry_table, FILE *stream);

void free_entry_table(vector* entry_table);

#endif /* _ENTRY_LIST_H_ */
