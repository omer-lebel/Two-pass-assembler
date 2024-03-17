/*
 Created by OMER on 3/6/2024.
*/


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
    unsigned param_types :4;
    Addressing_Mode add_mode;
    char symbol_name[MAX_LINE_LENGTH];
    Node *symbol;
    int val; /*imm or reg num*/
} Operand;

typedef struct op_analyze
{
    Opcode opcode;
    Operand src;
    Operand target;
    size_t address;

} op_analyze;

typedef enum lineType {
    str_l,
    data_l,
    ext_l,
    ent_l,
    op_l,
    def_l
}lineType;

typedef struct LineInfo
{
    lineType type_t;
    LinePart *parts;
    char label[MAX_LINE_LENGTH];
    Bool error_t;

    union {
        op_analyze *op;

        struct  {
            int arr[MAX_DATA_ARR_LENGTH];
            unsigned len;
        } data;

        struct {
            char content[MAX_STR_LENGTH];
            unsigned len;
        } str;

        struct {
            char name[MAX_LINE_LENGTH];
            int val;
        } define;

        struct {
            char name[MAX_LINE_LENGTH];
        }ext_ent;
    };

} LineInfo;

/****************** op list *******************/
void init_op_analyze (op_analyze *op, Opcode opcode);
vector *init_op_list(void);
op_analyze *add_to_op_list(vector* op_list, op_analyze *op);
size_t calc_op_size(op_analyze *op);
void print_op_analyze(op_analyze *op);
void print_op_list(vector *op_list, char* file_name);
void free_op_list(vector *op_list);


void print_line_info(LineInfo *line, char* file_name);

#endif /* _AST_H_ */
