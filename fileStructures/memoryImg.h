/*
 Created by OMER on 1/24/2024.
*/


#ifndef _MEMORY_IMG_H_
#define _MEMORY_IMG_H_

#include "../setting.h"
#include "../utils/text.h"
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
    int val;
} DsWord;

typedef struct Operand
{
    Addressing_Mode add_mode;
    Node *symbol;
    int val; /*imm or reg num*/
    Bool seen;
} Operand;

typedef struct op_analyze
{
    Op_Propriety *propriety;
    Operand src;
    Operand target;

    LineInfo *line_info;
    Bool errors;

} op_analyze;

exit_code init_data_seg (void);

exit_code addToDataSeg (LineInfo *line, DsType type, void *arr, size_t size);

void printDs (void);

void print_op_analyze(op_analyze *op);

/****************** text segment *******************/




/****************** extern *******************/
extern char *text_seg;
extern DsWord *data_seg;
extern size_t DC, IC;

#endif /*_MEMORY_IMG_H_ */
