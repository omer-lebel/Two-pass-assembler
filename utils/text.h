/**
 * @file text.h
 * @brief Provides essential text manipulation utilities and tokenizer.
 *
 * Includes functions for trimming whitespace, checking alphanumeric characters,
 * reading lines from files, and tokenizing lines into prefix, token, and
 * postfix parts using the "LineParts" structure.
*/

#ifndef _TEXT_H_
#define _TEXT_H_

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "../setting.h"

#define IS_EMPTY(s) (*s == '\0')
#define RESET_STR(str) ((str)[0] = '\0')
#define NULL_TERMINATE(str, index) ((str)[index] = '\0')
#define REMOVE_LAST_CHAR(str) ((str)[strlen(str) - 1] = '\0')
#define IS_LABEL(str) (str[strlen (str) - 1] == ':')

/**
 * @struct LinePart
 * @brief Struct representing parts of a line in the assembler code.
 */
typedef struct LinePart
{
    char *file;               /**< Pointer to the name of the file containing the line. */
    char line[MAX_LINE_LEN];  /**< The line of code. */
    char *prefix;             /**< Pointer to the prefix part of the line. */
    char *token;              /**< Pointer to the token part of the line. */
    char *postfix;            /**< Pointer to the postfix part of the line. */
    int num;                /**< Number of the line in the file. */

} LineParts;

/**
 * @brief Trims whitespace characters from the end of a string.
 *
 * @param str The string to trim.
 */
void trim_end (char *str);

/**
 * @brief Checks if a given string is a saved word.
 *
 * @param s The string to check.
 * @return TRUE if the string is a saved word, FALSE otherwise.
 */
Bool is_saved_word (const char *s);

/**
 * @brief Checks if a given string contains only alphanumeric characters.
 *
 * @param str The string to check.
 * @return TRUE if the string contains only alphanumeric characters, FALSE otherwise.
 */
Bool is_alpha_numeric (const char *str);

/**
 * @brief Reads a line from a file and stores it in a buffer.
 *
 * @param file     The file to read from.
 * @param buffer   The buffer to store the line in.
 * @param buff_size The size of the buffer.
 * @param overflow Pointer to a boolean variable indicating whether overflow occurred.
 * @return         A pointer to the buffer if a line is read, NULL otherwise.
 */
char *get_line (FILE *file, char *buffer, size_t buff_size, Bool *overflow);

/**
 * @brief Tokenizes a line into its prefix, token, and postfix parts.
 *
 * @param line The LineParts structure containing the line to tokenize.
 */
void lineTok (LineParts *line);

/**
 * @brief Initializes a LineParts structure with provided buffers.
 *
 * @param line_part        Pointer to the LineParts structure to initialize.
 * @param prefix_buffer    Buffer for the prefix part of the line.
 * @param token_buffer     Buffer for the token part of the line.
 * @param postfix_buffer   Buffer for the postfix part of the line.
 */
void init_line_parts (LineParts *line_part, char *prefix_buf,
                      char *token_buf, char *postfix_buf);

/**
 * @brief Restarts a LineParts structure by resetting prefix and token parts
 * to empty strings,
 * and copying the postfix part from the original line.
 *
 * @param line_part Pointer to the LineParts structure to restart.
 */
void restart_line_parts (LineParts *line_part);

/**
 * @brief Tokenizes a line into its identifier part.
 *
 * @param line      The LineParts structure containing the line to tokenize.
 * @param has_label Flag indicating whether the line has a label.
 */
void get_identifier_tok (LineParts *line, Bool has_label);

/**
 * @brief Tokenizes a line into its postfix part, used after processing a
 * label if present.
 *
 * @param line The LineParts structure containing the line to process.
 */
void line_to_postfix (LineParts *line);

/**
 * @brief Moves the first character of the token part to the prefix part of
 * the line.
 *
 * @param line The LineParts structure containing the line to process.
 */
void move_one_char_to_prefix (LineParts *line);

/**
 * @brief Splits a line into its prefix, token, and postfix parts based on a
 * provided token.
 *
 * @param parts       The LineParts structure containing the line to split.
 * @param wanted_tok  The token to split the line at.
 */
void split_line (LineParts *parts, char *wanted_tok);

#endif /* _TEXT_H_ */
