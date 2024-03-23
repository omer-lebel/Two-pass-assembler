#ifndef _FILE_STRUCTURES_H_
#define _FILE_STRUCTURES_H_

#include "../utils/vector.h"
#include "symbolTable.h"
#include "memoryImg.h"


typedef struct file_analyze{
    char file_name[1000]; /* todo change size*/
    Symbol_Table *symbol_table;
    Data_Segment *data_segment;
    Op_List *op_list;
    Entry_List *entry_list;
    int IC;
    int DC;
    Bool error;
}file_analyze;
/*
#ifndef FREE_FUNC
#define FREE_FUNC
void free_file_analyze (file_analyze *f)
{
  free_symbol_table (f->symbol_table);
  free_op_list (f->op_list);
  free_data_segment (f->data_segment);
  free_entry_list (f->entry_list);
  memset (f, 0, sizeof (file_analyze));
}
#endif*/

#define free_file_analyze(f)  \
    free_symbol_table((f)->symbol_table); \
    free_op_list((f)->op_list); \
    free_data_segment((f)->data_segment); \
    free_entry_list((f)->entry_list); \
    memset((f), 0, sizeof(file_analyze)); \

#endif /* _FILE_STRUCTURES_H_ */

