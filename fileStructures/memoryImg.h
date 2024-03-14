/*
 Created by OMER on 1/24/2024.
*/


#ifndef _MEMORY_IMG_H_
#define _MEMORY_IMG_H_

#include "../setting.h"
#include "../utils/vector.h"
#include "../utils/text.h"
#include "../utils/machineWord.h"
#include "../fileStructures/ast.h"
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


vector* init_data_seg(size_t *curr_DC);

exit_code add_to_data_seg (vector *data_segment, size_t *curr_DC ,
                           LineInfo* line, DsType type, void *arr, size_t
                           size);

void print_data_segment (vector *data_segment, size_t cur_DC);


/****************** text segment *******************/


vector* init_code_seg(size_t IC);

void *add_to_code_seg (vector *code_segment, op_analyze *op);

void print_code_segment_binary(vector* code_segment);

void print_code_segment(vector* code_segment);

void print_memory_img(vector* code_segment, vector* data_segment,
                      FILE *stream);


#endif /*_MEMORY_IMG_H_ */
