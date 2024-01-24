//
// Created by OMER on 1/23/2024.
//

#include "fsm.h"
#include "utils/linkedList.h"

Bool is_reg(char* str, int* val){
  if (str[0] == 'r' && str[1] >= '0' && str[1] <= '7' && str[2] == '\0'){
    *val = str[1] - 0;
    return TRUE;
  }
  *val = NO_REG;
  return FALSE;
}

Bool is_define_imm(char* str, int* val);

Bool is_imm(char* str, int* val);

Bool is_index(char* str, int* val);

Bool is_label(char* str, int* val);

exit_code get_src_add_mode();

exit_code get_target_add_mode();




state src_handler(LineInfo *line);

state target_handler(LineInfo *line);

state comma_handler(LineInfo *line);

state extra_text_handler();

exit_code run_fsm();