//
// Created by OMER on 3/6/2024.
//

#ifndef _AST_H_
#define _AST_H_

#include "../setting.h"
#include "../utils/text.h"
#include "../utils/vector.h"
#include "../utils/linkedList.h"

typedef enum Operand_Type
{
    SRC,
    TARGET
} Operand_Type;

typedef struct Operand
{
    Operand_Type type;
    Addressing_Mode add_mode;
    Node *symbol; //todo delete
    char symbol_name[MAX_LINE_SIZE];
    int val; /*imm or reg num*/
} Operand;

typedef struct op_analyze
{
    Op_Propriety *propriety;
    Operand src;
    Operand target;
    LineInfo *line_info;
    Bool errors;

} op_analyze;

/****************** op list *******************/
vector *init_op_list(void);
op_analyze *add_to_op_list(vector* op_list, op_analyze *op);
size_t calc_op_size(op_analyze *op);
void print_op_analyze(op_analyze *op, char* file_name);
void print_op_list(vector *op_list, char* file_name);
void free_op_list(vector *op_list);

#endif //_AST_H_
