/**
 * @file errors.h
 * @brief This file handles printing of error, messages and warnings.
 */

#ifndef _ERRORS_H_
#define _ERRORS_H_

#include "text.h"

typedef enum Msg_Code{
    no_input_file_err,                  /* must give at least one file to process */
    memory_err,                         /* Memory allocation failure. program stopped */
    assembler_failed,                   /* failed */
    ended_successfully,                 /* ended_successfully (actually not an error) */
}Msg_Code;

typedef enum Warning_Code{
    inaccessible_data,                  /*variables may be inaccessible without label*/
    ignored_label,                      /* label is ignored */
    duplicate_declaration_warning,      /* has already declared in earlier line */
}Warning_Code;

typedef enum Error_Code {
    /* ----------------- error of few stages */
    extraneous_text_err,                /* extraneous text */
    starts_with_non_alphabetic_err,     /* starts with a non-alphabetic character */
    reserved_keyword_used_err,          /* is a reserved keyword that cannot be used as an identifier */
    redeclaration_err,                  /* redeclaration of */
    undeclared_err,                     /* undeclared */

    /* ----------------- Pre-assembler errors */
    line_length_exceeded_err,           /* line length is more than 80 characters */
    eof_in_macro_definition_err,        /* reached EOF in the middle of macro definition. Expected 'endmcr' */
    nested_macro_definition_err,        /* nested macro definition is not allowed */
    empty_macro_declaration_err,        /* empty macro declaration */
    unexpected_endmcr_err,              /* unexpected endmcr */

    /* First errors */
    exceeds_available_memory,           /* Program size exceeds available memory space (4096 bytes). */
    contain_non_alphanumeric,           /* contains non-alphanumeric characters */
    label_and_define_on_same_line_err,  /* label and '.define' cannot be declared on the same line */
    expected_colon_err,                 /* expected ':' after */
    undefined_command_err,              /* undefined: */
    label_instead_of_imm_err,           /* expected numeric value or defined constant, but got label */
    exceeds_integer_bounds,             /* exceeds integer bounds [-(2^13), (2^13-1)]" */
    invalid_imm_err,                    /* expected numeric value or defined constant, but got: */
    expected_imm_after_sign_err,        /* expected numeric value or defined constant after */
    expected_bracket_got_nothing_err,   /* expected ']' */
    expected_bracket_before_expression_err,     /* expected ']' before */
    expected_int_before_bracket,        /* expected numeric value or defined constant before */
    empty_string_declaration_err,       /* empty string declaration */
    missing_opening_quote_err,          /* missing opening " character in */
    missing_terminating_quote_err,      /* missing terminating " character in */
    invalid_operand_err,                /* invalid operand */
    invalid_addressing_mode_err,        /* passing is invalid addressing mode to function */
    too_few_arguments_err,              /* too few arguments in instruction */
    expected_expression_before_comma_err,       /* expected expression before */
    expected_comma_before_expression_err,       /* expected ',' before */
    unexpected_comma_after_end_err,     /* unexpected ',' after end of command */
    empty_data_declaration_err,         /* empty data initializer */
    expected_integer_before_comma_err,  /* expected numeric value or defined constant before ',' */
    expected_identifier_err,            /* expected identifier after directive */
    empty_define_declaration_err,       /* empty define declaration */
    expected_equals_before_expression_err,       /* expected '=' before numeric token, but got */
    expected_numeric_expression_after_equal_err, /* expected numeric expression after '=' */
    invalid_numeric_expression_err,     /* is not a valid numeric expression */
} Error_Code;

/**
 * @brief Raises a general message based on the message code.
 *
 * @param msg_code Code of the message to raise.
 */
void raise_general_msg (Msg_Code msg_code);

/**
 * @brief Raises an open file error.
 *
 * @param file_name Name of the file that caused the error.
 */
void raise_open_file_error (char *file_name);

/**
 * @brief Raises a warning based on the warning code and line details.
 *
 * @param warning_code Code of the warning to raise.
 * @param line         Pointer to the line details.
 */
void raise_warning(Warning_Code warning_code, LineParts *line);

/**
 * @brief Raises an error based on the error code and line details.
 *
 * @param error_code Code of the error to raise.
 * @param line       Pointer to the line details.
 */
void raise_error(Error_Code error_code, LineParts *line);

/**
 * @brief Raises a second pass error for a specified token.
 *
 * @param line_parts Pointer to the line details.
 * @param wanted_tok The token that was expected.
 * @param file_name  Name of the file.
 */
void raise_second_pass_err (LineParts *line_parts, char* wanted_tok,
                            char* file_name);

#endif /*_ERRORS_H_*/
