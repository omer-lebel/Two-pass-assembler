/*
 Created by OMER on 1/23/2024.
*/


#include "setting.h"

Op_Propriety op_propriety[NUM_OF_OP];

char *SavedWord[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
                     ".data", ".string", ".entry", ".extern", ".define",
                     "mov", "cmp", "add", "sub", "lea",
                     "not", "clr", "inc", "dec", "jmp", "bne", "red", "prn",
                     "jsr", "rts", "hlt",
                     ""};


void init_op_propriety (Opcode opcode, char* op_name, ...)
{
  int i;
  va_list args; /* point to each unnamed arg in turn */
  Addressing_Mode mode;
  Op_Propriety *op = &op_propriety[opcode];

  op->opcode = opcode;
  strcpy (op->name, op_name);

  va_start(args, op_name); /* args point to the first unnamed arg */
  for (i = 0 ; i < 4 ; i++){
    mode = va_arg(args, Addressing_Mode); /* to the next unnamed arg */
    op->src_modes[i] = (mode != NONE_ADD);
  }
  for (i = 0 ; i < 4 ; i++){
    mode = va_arg(args, Addressing_Mode); /* to the next unnamed arg */
    op->target_modes[i] = (mode != NONE_ADD);
  }
  va_end(args);
}

void init_assembler_setting ()
{
  /* ----------------- first group - 2 operands ----------------- */

  init_op_propriety (MOV, "mov",
                     IMM_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD,
                     NONE_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD);

  init_op_propriety (CMP, "cmp",
                     IMM_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD,
                     IMM_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD);

  init_op_propriety (ADD, "add",
                     IMM_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD,
                     NONE_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD);

  init_op_propriety (SUB, "sub",
                     IMM_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD,
                     NONE_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD);

  init_op_propriety (LEA, "lea",
                     NONE_ADD, DIRECT_ADD, INDEX_ADD, NONE_ADD,
                     NONE_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD);


  /* ----------------- second group - 1 operators ----------------- */

  init_op_propriety (NOT, "not",
                     NONE_ADD, NONE_ADD, NONE_ADD, NONE_ADD,
                     NONE_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD);

  init_op_propriety (CLR, "clr",
                     NONE_ADD, NONE_ADD, NONE_ADD, NONE_ADD,
                     NONE_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD);

  init_op_propriety (INC, "inc",
                     NONE_ADD, NONE_ADD, NONE_ADD, NONE_ADD,
                     NONE_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD);

  init_op_propriety (DEC, "dec",
                     NONE_ADD, NONE_ADD, NONE_ADD, NONE_ADD,
                     NONE_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD);

  init_op_propriety (JMP, "jmp",
                     NONE_ADD, NONE_ADD, NONE_ADD, NONE_ADD,
                     NONE_ADD, DIRECT_ADD, NONE_ADD, REG_ADD);

  init_op_propriety (BNE, "bne",
                     NONE_ADD, NONE_ADD, NONE_ADD, NONE_ADD,
                     NONE_ADD, DIRECT_ADD, NONE_ADD, REG_ADD);

  init_op_propriety (RED, "red",
                     NONE_ADD, NONE_ADD, NONE_ADD, NONE_ADD,
                     NONE_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD);

  init_op_propriety (PRN, "prn",
                     NONE_ADD, NONE_ADD, NONE_ADD, NONE_ADD,
                     IMM_ADD, DIRECT_ADD, INDEX_ADD, REG_ADD);

  init_op_propriety (JSR, "jsr",
                     NONE_ADD, NONE_ADD, NONE_ADD, NONE_ADD,
                     NONE_ADD, DIRECT_ADD, NONE_ADD, REG_ADD);


  /* ----------------- third group - NONE_MODE operators ----------------- */

  init_op_propriety (RTS, "rts",
                     NONE_ADD, NONE_ADD, NONE_ADD, NONE_ADD,
                     NONE_ADD, NONE_ADD, NONE_ADD, NONE_ADD);

  init_op_propriety (HLT, "hlt",
                     NONE_ADD, NONE_ADD, NONE_ADD, NONE_ADD,
                     NONE_ADD, NONE_ADD, NONE_ADD, NONE_ADD);

}
