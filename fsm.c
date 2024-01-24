//
// Created by OMER on 1/23/2024.
//

#include "fsm.h"
#include "utils/linkedList.h"
#define MAX_IMM (2^11 -1)
#define MIN_IMM -(2^11 -1)

Bool is_reg(char* str, int* val){
  if (str[0] == 'r' && str[1] >= '0' && str[1] <= '7' && str[2] == '\0'){
    *val = str[1] - 0;
    return TRUE;
  }
  *val = NO_REG;
  return FALSE;
}

Bool is_int2 (char* str, long int *res){
  char *end_ptr = NULL;
  *res = strtol (str, &end_ptr, 10);
  if (!IS_EMPTY(end_ptr)){
    return FALSE;
  }
  return TRUE;
}

/*todo macro*/
Bool is_int_sign(char* str){
  return str[0] == '#';
}

Bool within_imm_bound(long int num){
  return num >= MIN_IMM && num <= MAX_IMM;
}

Bool is_imm2(LineInfo *line, int *res){
  long int tmp;
  if (is_int_sign(line->token) && is_int2 (line->token, &tmp)){
    if (!within_imm_bound (tmp)){
      /*todo check if need that?*/
      r_error ("", line, " exceeds immediate bounds [-(2^11-1), 2^11-1]");
    }
    *res = (int) tmp;
    return TRUE;
  }
  return FALSE;
}















//Bool is_define_imm(char* str, int* val);
//
//Bool is_imm(char* str, int* val);
//
//Bool is_index(char* str, int* val);
//
//Bool is_label(char* str, int* val);
//
//exit_code get_src_add_mode();
//
//exit_code get_target_add_mode();




//state src_handler(LineInfo *line);
//
//state target_handler(LineInfo *line);
//
//state comma_handler(LineInfo *line);
//
//state extra_text_handler();
//
//exit_code run_fsm();