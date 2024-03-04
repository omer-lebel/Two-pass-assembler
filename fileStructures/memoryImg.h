/*
 Created by OMER on 1/24/2024.
*/


#ifndef _MEMORY_IMG_H_
#define _MEMORY_IMG_H_

#include "../setting.h"
#include "../utils/text.h"
#include "../fileStructures/symbolTable.h"

#include "../utils/vector.h"

#define MACHINE_WORD_SIZE 14
/****************** data segment *******************/

typedef enum DsType
{
    CHAR_TYPE,
    INT_TYPE
} DsType;

typedef struct DsWord
{
    DsType type; /*todo debug*/
    unsigned int val: 14;
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

//extern size_t IC;
//extern op_analyze *text_seg;

vector* init_text_seg(size_t *curr_IC);

/*------------------------------------*/

void print_op_analyze(op_analyze *op);



/****************** extern *******************/


#endif /*_MEMORY_IMG_H_ */
