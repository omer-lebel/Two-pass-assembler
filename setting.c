//
// Created by OMER on 1/23/2024.
//

#include "setting.h"

Op_Info op_info[NUM_OF_OP];

void init_op (Opcode opcode, char* op_name,...)
{
  int i;
  va_list args; /* point to each unnamed arg in turn */
  Operand_Add_Mode mode;

  op_info->opcode = opcode;
  strcpy (op_info->name, op_name);

  va_start(args, op_name); /* args point to the first unnamed arg */
  for (i = 0 ; i < 4 ; i++){
    mode = va_arg(args, Operand_Add_Mode); /* to the next unnamed arg */
    op_info->src_modes[i] = (mode != NONE_MODE);
  }
  for (i = 0 ; i < 4 ; i++){
    mode = va_arg(args, Operand_Add_Mode); /* to the next unnamed arg */
    op_info->target_modes[i] = (mode != NONE_MODE);
  }
  va_end(args);
}

void init_assembler_setting ()
{
  /* ----------------- first group - 2 operands ----------------- */

  init_op(MOV, "mov",
          IMMEDIATE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE,
          NONE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE);

  init_op(CMP, "cmp",
          IMMEDIATE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE,
          NONE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE);

  init_op(ADD, "add",
          IMMEDIATE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE,
          NONE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE);

  init_op(SUB, "sub",
          IMMEDIATE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE,
          NONE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE);

  init_op(LEA, "lea",
          NONE_MODE, DIRECT_MODE, INDEX_MODE, NONE_MODE,
          NONE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE);


  /* ----------------- second group - 1 operators ----------------- */

  init_op(NOT, "not",
          NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE,
          NONE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE);

  init_op(CLR, "clr",
          NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE,
          NONE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE);

  init_op(INC, "inc",
          NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE,
          NONE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE);

  init_op(DEC, "dec",
          NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE,
          NONE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE);

  init_op(JMP, "jmp",
          NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE,
          NONE_MODE, DIRECT_MODE, NONE_MODE, REGISTER_MODE);

  init_op(BNE, "bne",
          NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE,
          NONE_MODE, DIRECT_MODE, NONE_MODE, REGISTER_MODE);

  init_op(RED, "red",
          NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE,
          NONE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE);

  init_op(PRN, "prn",
          NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE,
          IMMEDIATE_MODE, DIRECT_MODE, INDEX_MODE, REGISTER_MODE);

  init_op(JSR, "jsr",
          NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE,
          NONE_MODE, DIRECT_MODE, NONE_MODE, REGISTER_MODE);


  /* ----------------- third group - NONE_MODE operators ----------------- */

  init_op(RTS, "rts",
          NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE,
          NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE);

  init_op(HLT, "hlt",
          NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE,
          NONE_MODE, NONE_MODE, NONE_MODE, NONE_MODE);

}
