/*
 Created by OMER on 1/24/2024.
*/


#ifndef _MEMORY_IMG_H_
#define _MEMORY_IMG_H_

#include "../setting.h"
#include "../utils/text.h"
#include "../fileStructures/symbolTable.h"

#include "../utils/vector.h"



/****************** data segment *******************/

typedef enum DsType
{
    CHAR_TYPE,
    INT_TYPE
} DsType;

typedef struct DsWord
{
    DsType type; /*todo only if debug*/
    int val: 14; /* todo 14? */
} DsWord;

typedef enum Operand_Type
{
    SRC,
    TARGET
} Operand_Type;

typedef struct Operand
{
    Operand_Type type;
    Addressing_Mode add_mode;
    Node *symbol;
    char symbol_name[MAX_LINE_SIZE];
    int val; /*imm or reg num*/
    Bool seen;
} Operand;


/*------------------------------------*/

vector* init_data_seg(size_t *curr_DC);

exit_code add_to_data_seg (vector *data_segment, size_t *curr_DC ,
                           LineInfo* line, DsType type, void *arr, size_t
                           size);

void print_data_segment (vector *data_segment, size_t cur_DC);


/****************** text segment *******************/
typedef struct op_analyze
{
    Op_Propriety *propriety;
    Operand src;
    Operand target;

    LineInfo *line_info;
    Bool errors;

} op_analyze;

vector* init_code_seg(size_t IC);
void *add_to_code_seg (vector *code_segment, op_analyze *op);
void print_code_segment(vector* code_segment);


/****************** op list *******************/
vector *init_op_list(void);
op_analyze *add_to_op_list(vector* op_list, op_analyze *op);
size_t calc_op_size(op_analyze *op);
void print_op_analyze(op_analyze *op, char* file_name);
void print_op_list(vector *op_list, char* file_name);
void free_op_list(vector *op_list);

#endif /*_MEMORY_IMG_H_ */
