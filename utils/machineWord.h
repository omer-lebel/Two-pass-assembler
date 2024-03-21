/*
 Created by OMER on 3/2/2024.
*/


#ifndef _MACHINE_WORD_H_
#define _MACHINE_WORD_H_

#include <stdio.h>
#include "../setting.h"


unsigned short first_word(int opcode, int add_mode_src, int add_mode_target);
unsigned short registers_word(int src, int target);
unsigned short imm_word(int imm);
unsigned short label_word(int address, int are);
void print_binary_word(unsigned short word);
void print_special_base_word(unsigned short binaryWord, Bool is_last, FILE *stream);


#endif /* _MACHINE_WORD_H_ */
