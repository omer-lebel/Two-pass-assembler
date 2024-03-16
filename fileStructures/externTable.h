/*
 Created by OMER on 3/6/2024.
*/


#ifndef _EXTERN_TABLE_H_
#define _EXTERN_TABLE_H_

#include "../setting.h"
#include "../utils/vector.h"

typedef struct ExternUse{
    char name[MAX_LINE_LENGTH];
    vector *location;
}ExternSyb;

vector * init_extern_table(void);

ExternSyb* add_to_extern_table(vector* extern_table, char* name,
                               size_t address);

size_t* add_location(ExternSyb *extern_syb, size_t address);

ExternSyb* find_extern_syb(vector* extern_table, char* name);

void print_extern_table (vector *extern_table, FILE *file);

void free_extern_table(vector* extern_table);





#endif /* _EXTERN_TABLE_H_ */
