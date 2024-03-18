/*
 Created by OMER on 3/6/2024.
*/


#ifndef _ANALYZER_H_
#define _ANALYZER_H_

#include "../setting.h"
#include "../utils/text.h"
#include "../utils/vector.h"
#include "../utils/linkedList.h"

typedef enum Operand_Type
{
    SRC = 1,
    TARGET
} Operand_Type;


typedef struct Operand
{
    Operand_Type type;
    unsigned param_types: 4;
    Addressing_Mode add_mode;
    union
    {
        int reg_num;            /* register mode */
        int imm;                /* imm mode */
        struct {
            void *symbol;       /* symbol or index mode */
            Bool found;
            int offset;         /* offset is 0 in direct mode */
        };
    };
} Operand;

typedef struct op_analyze
{
    Opcode opcode;
    Operand src;
    Operand target;
    int address;

} op_analyze;

typedef enum lineType
{
    str_l,
    data_l,
    ext_l,
    ent_l,
    op_l,
    def_l
} lineType;

/*union
{
    op_analyze *op;
    int *data_arr;
    char *string;
    char *dir_name;
};

union{
    int data_arr_len;
    int str_len;
    int define_val;
};*/

typedef struct LineInfo
{
    lineType type_t;
    LinePart *parts;
    char *label;

    union
    {
        op_analyze *op;    /** operator */

        struct {            /** .data */
            int *arr;
            int len;
        } data;

        struct {            /** .string */
            char *content;
            int len;
        } str;

        struct {             /** .define */
            char *name;
            int val;
        } define;

        struct {             /** .entry | .extern */
            char *name;
        } ext_ent;
    };

} LineInfo;

/****************** op list *******************/
void init_op_analyze (op_analyze *op, Opcode opcode, char *src_sym_buffer,
                      char * target_sym_buffer);
vector *init_op_list (void);
op_analyze *add_to_op_list (vector *op_list, op_analyze *op);
int calc_op_size (op_analyze *op);
void print_op_analyze (op_analyze *op);
void print_op_list (vector *op_list, char *file_name);
void free_op_list (vector *op_list);

void print_line_info (LineInfo *line, char *file_name);

#endif /* _ANALYZER_H_ */
