/**
 * @file machineWord.h
 * @brief This file provides functions for constructing machine words of 14 bits
 * representing instructions, registers, immediate values, and labels
 * in the machine language. Also includes functions for printing
 * binary and special base representations of machine words.
 */
#ifndef _MACHINE_WORD_H_
#define _MACHINE_WORD_H_

#include "../setting.h"

/**
 * @brief Constructs the first word of an instruction in the machine language.
 *
 * @param opcode            Opcode of the instruction.
 * @param add_mode_src      Addressing mode of the source operand.
 * @param add_mode_target   Addressing mode of the target operand.
 * @return The constructed machine word.
 */
unsigned short first_word (int opcode, int add_mode_src, int add_mode_target);

/**
 * @brief Constructs a word representing register operand/s in the machine
 * language.
 *
 * @param src    Source register
 * @param target Target register.
 * @return The constructed machine word.
 */
unsigned short registers_word (int src, int target);

/**
 * @brief Constructs a word representing an immediate value in the machine
 * language.
 *
 * @param imm Immediate value.
 * @return The constructed machine word.
 */
unsigned short imm_word (int imm);

/**
 * @brief Constructs a word representing a label in the machine language.
 *
 * @param address Address of the label.
 * @param are     A,R,E bits indicating addressing mode.
 * @return The constructed machine word.
 */
unsigned short label_word (int address, int are);

/**
 * @brief Prints a binary representation of the given machine word.
 *
 * @param word   The machine word to be printed.
 */
void print_binary_word (unsigned short word);

/**
 * @brief Prints a special base representation of the given machine word.
 *
 * @param binaryWord The machine word to be printed.
 * @param is_last    Flag indicating if the word is the last one in a sequence.
 * @param stream     File stream to print the representation to.
 */
void
print_special_base_word (unsigned short binaryWord, Bool is_last, FILE *stream);

#endif /* _MACHINE_WORD_H_ */
