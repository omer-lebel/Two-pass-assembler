/*
 Created by OMER on 1/23/2024.
*/


#ifndef _FSM_H_
#define _FSM_H_

#include "setting.h"
#include "utils/text.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/memoryImg.h"


/* states */
typedef enum state
{
    SRC_STATE,
    TARGET_STATE,
    COMA_STATE,
    EXTRA_TEXT_STATE,
    END_STATE,
    ERROR_STATE
} state;

typedef state (*handler) ( op_analyze *op, state nextState);

typedef struct transition
{
    state from;
    handler handler;
    state next;
} transition;


state src_handler ( op_analyze *op, state nextState);
state comma_handler ( op_analyze *op, state nextState);
state target_handler ( op_analyze *op, state nextState);
state extra_text_handler ( op_analyze *op, state nextState);

int run_fsm (op_analyze *op);

#endif /* _FSM_H_ */
