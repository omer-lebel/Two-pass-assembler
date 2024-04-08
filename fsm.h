/*
 Created by OMER on 1/23/2024.
*/


#ifndef _FSM_H_
#define _FSM_H_

#include "setting.h"
#include "utils/text.h"
#include "fileStructures/symbolTable.h"
#include "fileStructures/analyzer.h"
#include "fileStructures/dataSeg.h"
#include "fileStructures/codeSeg.h"

/* states */
typedef enum state
{
    SRC_STATE,
    COMA_STATE,
    TARGET_STATE,

    STRING_STATE,

    IMM_STATE,

    IDENTIFIER_STATE,
    EQUAL_STATE,
    INT_STATE,

    EXTRA_TEXT_STATE,
    END_STATE,
    ERROR_STATE
} state;

typedef state (*handler) (LineInfo *line, Symbol_Table *table, state nextState);

typedef struct transition
{
    state from;
    handler handler;
    state next;
} transition;

Bool is_valid_identifier (LineParts *line, char *name, Bool print_err);

state str_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state);
state imm_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state);

state identifier_handler (LineInfo *line, Symbol_Table *symbol_table, state
next_state);
state equal_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state);
state int_handler (LineInfo *line, Symbol_Table *symbol_table, state next_state);

state src_handler (LineInfo *line, Symbol_Table *table, state nextState);
state comma_handler (LineInfo *line, Symbol_Table *table, state nextState);
state target_handler (LineInfo *line, Symbol_Table *symbol_table, state nextState);
state extra_text_handler (LineInfo *line, Symbol_Table *symbol_table, state
nextState);

Bool run_fsm (LineInfo *line, Symbol_Table *symbol_table);

#endif /* _FSM_H_ */
