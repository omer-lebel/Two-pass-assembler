/*
 Created by OMER on 1/23/2024.
*/

#include "setting.h"

char *SavedWord[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
                     ".data", ".string", ".entry", ".extern", ".define",
                     "mov", "cmp", "add", "sub", "lea",
                     "not", "clr", "inc", "dec", "jmp", "bne", "red", "prn",
                     "jsr", "rts", "hlt",
                     "mcr", "endmcr", ""};

char *op_names[NUM_OF_OP] = {"mov", "cmp", "add", "sub", "not", "clr",
                             "lea", "inc", "dec", "jmp", "bne", "red",
                             "prn","jsr", "rts", "hlt"};

unsigned int param_types[NUM_OF_OP][3] =
    {{MOV, all_mode,       symbol_n_index_n_reg}, /* | 1111 | 0111 | */
     {CMP, all_mode,       all_mode},             /* | 1111 | 1111 | */
     {ADD, all_mode,       symbol_n_index_n_reg}, /* | 1111 | 0111 | */
     {SUB, all_mode,       symbol_n_index_n_reg}, /* | 1111 | 0111 | */
     {NOT, no_operand,     symbol_n_index_n_reg}, /* |   -  | 0111 | */
     {CLR, no_operand,     symbol_n_index_n_reg}, /* |   -  | 0111 | */
     {LEA, symbol_n_index, symbol_n_index_n_reg}, /* | 0110 | 0111 | */
     {INC, no_operand,     symbol_n_index_n_reg}, /* |   -  | 0111 | */
     {DEC, no_operand,     symbol_n_index_n_reg}, /* |   -  | 0111 | */
     {JMP, no_operand,     symbol_n_reg},         /* |   -  | 0101 | */
     {BNE, no_operand,     symbol_n_reg},         /* |   -  | 0101 | */
     {RED, no_operand,     symbol_n_index_n_reg}, /* |   -  | 0111 | */
     {PRN, no_operand,     all_mode},             /* |   -  | 1111 | */
     {JSR, no_operand,     symbol_n_reg},         /* |   -  | 0101 | */
     {INC, no_operand,     no_operand},           /* |   -  |   -  | */
     {INC, no_operand,     no_operand}};          /* |   -  |   -  | */

/*  --------------------------------------------------------------------
  * |      operator      |      scr operand      |    target operand     |
  * |--------------------|-----------------------|-----------------------|
  * |   addressing mode  | imm | sym | ind | reg | imm | sym | ind | reg |
  * |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  * |    mov, add, sub   |     |  X  |  X  |  X  |     |  X  |  X  |  X  |
  * |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  * |        cmp         |  X  |  X  |  X  |  X  |     |  X  |  X  |  X  |
  * |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  * |        lea         |     |  X  |  X  |     |  X  |  X  |  X  |  X  |
  * |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  * | not, clr, inc, dec |                       |  X  |  X  |  X  |  X  |
  * |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  * |    jmp, bne, jsr   |                       |     |  X  |     |  X  |
  * |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  * |        red         |                       |     |  X  |     |  X  |
  * |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  * |        prn         |                       |  X  |  X  |  X  |  X  |
  * |--------------------|-----+-----+-----+-----|-----+-----+-----+-----|
  * |     rts, hlt       |                       |                       |
  * ----------------------------------------------------------------------
  * */

FILE *open_file (char *file_name, const char *extension, const char *mode)
{
  FILE *tmp;
  size_t len = strlen (file_name);
  strcat (file_name, extension);
  tmp = fopen (file_name, mode);
  if (!tmp) {
    printf ("error while opening '%s'\n", file_name);
    return NULL;
  }
  else {
    file_name[len] = '\0'; /* remove extension */
    return tmp;
  }
}

void remove_file (char *file_name, const char *extension)
{
  size_t len = strlen (file_name);
  strcat (file_name, extension);
  remove (file_name);
  file_name[len] = '\0'; /* remove extension */
}
