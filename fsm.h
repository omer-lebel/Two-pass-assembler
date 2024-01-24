//
// Created by OMER on 1/23/2024.
//

#ifndef _FSM_H_
#define _FSM_H_

#include "utils/text.h"
#include "setting.h"


/* states */
typedef enum state
{
    SRC_STATE,
    COMA_STATE,
    TARGET_STATE,
    EXTRA_TEXT_STATE,
    END_STATE,
    ERROR_STATE
} state;

typedef state (*handler) ( LineInfo *line, state nextState);

typedef struct transition
{
    state from;
    handler handler;
    state next;
} transition;

#endif //_FSM_H_
