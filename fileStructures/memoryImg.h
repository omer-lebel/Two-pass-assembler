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


vector* init_data_seg(int *curr_DC);

Bool add_to_data_seg (vector *data_segment, int *DC,
                           DsType type, void *arr, int size);

void print_data_segment (vector *data_segment, int cur_DC);


/****************** text segment *******************/


vector* init_code_seg(int IC);

void *add_to_code_seg (vector *code_segment, op_analyze *op);

void print_code_segment_binary(vector* code_segment);

void print_code_segment(vector* code_segment);

void print_memory_img(vector* code_segment, vector* data_segment,
                      FILE *stream);


#endif /*_MEMORY_IMG_H_ */
