/*
 Created by OMER on 1/24/2024.
*/


#ifndef _MEMORY_IMG_H_
#define _MEMORY_IMG_H_

#include "../setting.h"
#include "../utils/vector.h"
#include "../utils/text.h"
#include "../utils/machineWord.h"
#include "../fileStructures/analyzer.h"
#include "../fileStructures/symbolTable.h"


/****************** data segment *******************/

typedef enum DsType
{
    CHAR_TYPE,
    INT_TYPE
} DsType;

typedef struct DsWord
{
    DsType type;
    int val: 14;
} DsWord;

typedef vector Data_Segment;

Data_Segment* new_data_segment(int *DC);

Bool add_to_data_segment (Data_Segment *data_segment, int *DC,
                          DsType type, void *arr, int size);

void show_data_segment (Data_Segment *data_segment, FILE *stream);

void free_data_segment (Data_Segment *data_segment);



void print_memory_img(Op_List * op_list, int ic, Data_Segment * data_segment,
                      int dc, FILE *stream);


#endif /*_MEMORY_IMG_H_ */
