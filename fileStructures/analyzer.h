/*
 Created by OMER on 3/6/2024.
*/


#ifndef _ANALYZER_H_
#define _ANALYZER_H_

#include "../setting.h"
#include "../utils/text.h"
#include "../utils/vector.h"
#include "symbolTable.h"
#include "codeSeg.h"

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

#endif /* _ANALYZER_H_ */
