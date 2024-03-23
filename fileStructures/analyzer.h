/*
 Created by OMER on 3/6/2024.
*/
/*
 * line info --> move to setting??
 * op_line and op_analyze
 */

#ifndef _ANALYZER_H_
#define _ANALYZER_H_

#include "../setting.h"
#include "../utils/text.h"
#include "../utils/vector.h"
#include "symbolTable.h"

/************************* op analyze ***************************/

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
        struct
        {                /** symbol or index mode */
            /* symbol fount - point to node in symbol table
            * not found - point temporary to char of its name */
            void *symbol;
            Bool found;
            int offset;         /* offset is 0 in direct mode */
        } symInx;
    } info;
} Operand;

typedef struct Op_Analyze
{
    Opcode opcode;
    int address;
    Operand src;
    Operand target;

} Op_Analyze;

void init_op_analyze (Op_Analyze *op, Opcode opcode, char *src_sym_buffer,
                      char *target_sym_buffer);

int calc_op_size (Op_Analyze *op);

/************************* line info ***************************/


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
    Op_Analyze *op;    /** operator */

    struct
    {            /** .data */
        int *arr;
        int len;
    } data;

    struct
    {            /** .string */
        char *content;
        int len;
    } str;

    struct
    {             /** .define */
        void *name;
        Bool found;
        int val;
    } define;

    struct
    {             /** .entry | .extern */
        void *name;
        Bool found;
    } ext_ent;
} Info;

typedef struct LineInfo
{
    lineType type_l;    /** str / data / ext / ent / def / op */
    LineParts *parts;    /** line, line num, postfix, prefix, postfix*/
    char *label;        /** if exist */
    Info info;          /** info according to tha type */

} LineInfo;

void print_line_info (LineInfo *line, char *file_name);

/************************* op list ***************************/
typedef struct Op_Line{
    Op_Analyze *analyze;
    LineParts *parts;
}Op_Line;


typedef vector Op_List;


Op_List *new_op_list (void);

Op_Line *add_to_op_list (Op_List *op_list, Op_Analyze *op_analyze,
                         LineParts *line_part);


void show_op_list (Op_List *op_list, FILE *stream);

void free_op_list (Op_List *op_list);

/************************* entry list ***************************/



typedef  struct Entry_line{
    Symbol *symbol;
    LineParts *parts;   /*line will be null if the symbol resolved  in adding */
}Entry_line;

typedef vector Entry_List;

Entry_List *new_entry_list (void);

Entry_line *add_to_entry_list (Entry_List *entry_list, Symbol *symbol,
                               LineParts *line_part);

void print_entry_list (Entry_List *entry_list, FILE *stream);

void free_entry_list (vector *entry_list);

#endif /* _ANALYZER_H_ */
