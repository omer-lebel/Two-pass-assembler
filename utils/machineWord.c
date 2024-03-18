/*
 Created by OMER on 3/2/2024.
*/


#include "machineWord.h"
#include "../setting.h"

#define OPCODE_SHIFT 6
#define ADD_MOD_SRC_SHIFT 4
#define ADD_MOD_TARGET_SHIFT 2
#define PAIR 0x3


unsigned short first_word(int opcode, int add_mode_src, int
add_mode_target) {
  /* Create the machine word using bitwise operations */
  unsigned short machineWord = 0;

  /* bits 9-6: opcode */
  machineWord |= (opcode << OPCODE_SHIFT);

  /* bits 5-4: addressing mode of src operand */
  machineWord |= (add_mode_src << ADD_MOD_SRC_SHIFT);

  /* bits 3-2: addressing mode of target operand */
  machineWord |= (add_mode_target << ADD_MOD_TARGET_SHIFT);

  /* bits 1-0: A,R,E always 00 at the first word */

  return machineWord;
}

unsigned short registers_word(int src, int target) {
  /* Create the machine word using bitwise operations */
  unsigned short machineWord = 0;

  /* bits 7-5: src register */
  machineWord |= (src << 5);

  /* bits 4-2: target register */
  machineWord |= (target << 2);

  /* bits 1-0: A,R,E(always 00 at register word */

  return machineWord;
}

unsigned short imm_word(int imm) {
  /* Create the machine word using bitwise operations */
  unsigned short machineWord = 0;

  /* bits 13-2: src register */
  machineWord |= (imm << 2);

  return machineWord;
}

unsigned short label_word(int address, int are) {
  /* Create the machine word using bitwise operations */
  unsigned short machineWord = 0;

  /* bits 13-2: label's address */
  machineWord |= (address << 2);

  /* bits 1-0: A,R,E */
  machineWord |= (are);

  return machineWord;
}


void print_binary_word(unsigned short word) {
  int i;
  for (i = MACHINE_WORD_SIZE - 1; i >= 0; --i) {
    printf("%d", (word >> i) & 1);
  }
  printf("\n");
}

void print_special_base_word(unsigned short binaryWord, FILE *stream) {
  int i;
  char c;
  unsigned int pair;
  for (i = MACHINE_WORD_SIZE - 2 ; i >= 0; i -= 2) {
    pair = (binaryWord >> i) & PAIR;
    switch (pair) {
      case 0:
        c = '*';
        break;
      case 1:
        c = '#';
        break;
      case 2:
        c = '%';
        break;
      case 3:
        c = '!';
        break;
      default:
        c = '-';
        break;
    }
    fputc (c, stream);
  }
}