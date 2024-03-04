//
// Created by OMER on 3/4/2024.
//

#ifndef _PRE_SECOND_H_
#define _PRE_SECOND_H_

#include "../setting.h"
#include "../utils/text.h"
#include "symbolTable.h"

typedef struct appear{
    LineInfo *line;
    int code_seg_ind;

}appear;

typedef struct Unrecognized_Label{
    Symbol *symbol;
    vector* appearances;

}Unrecognized_Label;

#endif //_PRE_SECOND_H_
