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
    int IC;
    int DC;
    Bool error;
}file_analyze;

#endif /* _FILE_STRUCTURES_H_ */