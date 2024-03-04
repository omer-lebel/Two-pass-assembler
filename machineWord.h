//
// Created by OMER on 3/2/2024.
//

#ifndef _MACHINE_WORD_H_
#define _MACHINE_WORD_H_

unsigned short first_word(int opcode, int add_mode_src, int add_mode_target);
unsigned short registers_word(int src, int target);
unsigned short imm_word(int imm);
unsigned short label_word(int label, int are);


#endif //_MACHINE_WORD_H_
