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
    Node *symbol;
    char symbol_name[MAX_LINE_LENGTH];
    int val; /*imm or reg num*/
} Operand;

typedef struct op_analyze
{
    Opcode opcode;
    Operand src;
    Operand target;
    LinePart *line_part;
    size_t address;
    Bool errors; /* todo delete? */

} op_analyze;

typedef enum lineType {
    str_t,
    data_t,
    ext_t,
    ent_t,
    op_t
}lineType;

typedef struct LineInfo
{
    lineType type_t;
    LinePart *line_part;
    char label[MAX_LINE_LENGTH];
    Bool error_t;

    union {
        char content[MAX_LINE_LENGTH]; /* for str/ent/ext*/
        int int_arr[MAX_LINE_LENGTH];
        op_analyze *op;
    };

} LineInfo;

/****************** op list *******************/
void init_op_analyze (op_analyze *op, Opcode opcode, LinePart *line);
vector *init_op_list(void);
op_analyze *add_to_op_list(vector* op_list, op_analyze *op);
size_t calc_op_size(op_analyze *op);
void print_op_analyze(op_analyze *op, char* file_name);
void print_op_list(vector *op_list, char* file_name);
void free_op_list(vector *op_list);

#endif /* _AST_H_ */
