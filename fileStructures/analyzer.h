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
        int reg_num;            /** register mode */
        int imm;                /** imm mode */
        struct {                /** symbol or index mode */
            /* symbol fount - point to node in symbol table
            * not found - point temporary to char of its name */
            void *symbol;
            Bool found;
            int offset;         /* offset is 0 in direct mode */
        } symInx;
    } info;
} Operand;

typedef struct op_analyze
{
    Opcode opcode;
    int address;
    Operand src;
    Operand target;

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


/** info about the line according to its type */
typedef union Info
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
} Info;


typedef struct LineInfo
{
    lineType type_l;    /** str / data / ext / ent / def / op */
    LinePart *parts;    /** line, line num, postfix, prefix, postfix*/
    char *label;        /** if exist */
    Info info;          /** info according to tha type */

} LineInfo;



/****************** op list *******************/
void init_op_analyze (op_analyze *op, Opcode opcode, char *src_sym_buffer,
                      char * target_sym_buffer);
//vector *init_op_list (void);
//op_analyze *add_to_op_list (vector *op_list, op_analyze *op);
int calc_op_size (op_analyze *op);
void print_op_analyze (op_analyze *op);
void print_op_list (vector *op_list, char *file_name);
void free_op_list (vector *op_list);

void print_line_info (LineInfo *line, char *file_name);

#endif /* _ANALYZER_H_ */
