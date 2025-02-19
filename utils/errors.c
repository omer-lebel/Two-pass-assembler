#include "errors.h"

/* -------------------------- color and font style ------------------------- */
#define RED_COLOR   "\x1B[31m"  /* error's color */
#define YEL_COLOR   "\x1B[33m"        /* warning's color */
#define GRN_COLOR   "\x1B[32m"        /* success's color */
#define RESET_COLOR "\x1B[0m"         /* reset to regular color */

#define BOLD "\033[1m"          /* bolt font (for errors & warnings) */
#define REG "\033[0m"           /* reset to regular font */


/* ---------------------- helper function declaration ---------------------- */
void general_msg    (char *color, char *msg);
void r_error        (char *msg_before, LineParts *line, char *msg_after);
void r_warning      (char *msg_before, LineParts *line, char *msg_after);
void line_msg       (char *type, char *color, char *msg_before, LineParts *line, char *msg_after);

/* ------------------------------------------------------------------------- */

void raise_general_msg (Msg_Code msg_code)
{
  switch (msg_code) {
    case no_input_file_err:
      general_msg (RED_COLOR, "Error: must give at least one file to process");
      break;

    case memory_err:
      general_msg (RED_COLOR, "\n*** Memory allocation failure. program stopped.");
      break;

    case assembler_failed:
      general_msg (RED_COLOR, "\n *** Processing failed.");
      break;

    case ended_successfully:
      general_msg (GRN_COLOR, "Ended successfully\n");
      break;
  }
}

/**
 * @brief Prints a general message with the specified color.
 *
 * @param color Color of the message.
 * @param msg   Message to print.
 */
void general_msg (char *color, char *msg)
{
  printf ("%s%s\n" RESET_COLOR, color, msg);
}

void raise_open_file_error (char *file_name)
{
  printf (RED_COLOR "*** Error while opening '%s'. Stop current process. "
          "\n" RESET_COLOR, file_name);
}

void raise_warning (Warning_Code warning_code, LineParts *line)
{
  switch (warning_code) {

    case inaccessible_data:
      /* .data 1,2,3 | .string "hey" */
      line_to_postfix (line); /*get the first tok again for the error msg */
      r_warning ("", line, "variables may be inaccessible without label");
      break;

    case ignored_label:
      /* LABEL: .entry ENT | LABEL: .extern: EXT */
      line_to_postfix (line); /*get the fist tok again for the error msg */
      lineTok (line);
      r_warning ("label ", line, " is ignored");
      break;

    case duplicate_declaration_warning:
      /* .extern X
       * .extern X */
      r_warning ("", line, " has already declared in earlier line");
      break;

  }
}

void raise_error (Error_Code error_code, LineParts *line)
{
  switch (error_code) {
    /* =========================== preprocessor & first pass errors: */
    case extraneous_text_err:
      /* .extern X 123 | mcr MCR extra text */
      strcat (line->token, line->postfix); /*Concatenates the entire continuation */
      NULL_TERMINATE(line->postfix, 0);
      r_error ("unexpected text: ", line, "");
      break;

    case starts_with_non_alphabetic_err:
      /* 1L: .data 2,3 | .entry $EN | .define %x = 3 */
      r_error ("", line, " starts with a non-alphabetic character");
      break;

    case reserved_keyword_used_err:
      /* mov: .string "a" | .extern endmcr | .define .data = 2 */
      r_error ("", line, " is a reserved keyword that cannot be used as an "
                         "identifier");
      break;

    case redeclaration_err:
      /* redeclaration of label or of macro
       * note: * redeclaration of entry/extern is possible (gives warning)
       *       * redeclaration of define possible if it has the same val */
      r_error ("redeclaration of ", line, "");
      break;

      /* =========================== first & second errors: */
    case undeclared_err:
      /* use of undeclared constant or label (including .entry)*/
      if (line->token[0] == '#') {
        move_one_char_to_prefix (line);
      }
      r_error ("", line, " undeclared");
      break;

      /* =========================== preprocessor errors: */
    case line_length_exceeded_err:
      r_error ("line length is more than 80 characters", line, "");
      break;

    case eof_in_macro_definition_err:
      /* mcr M
       *    prn #2
       *    mov r0, r2
       */
      trim_end (line->postfix);
      r_error ("reached EOF in the middle of macro definition. Expected "
               "'endmcr'", line, "");
      break;

    case nested_macro_definition_err:
      /* mcr M1
       *    add r0, r2
       *    mcr M2
       *        jmp LOOP
       *    endmcr
       * endmcr   */
      r_error ("", line, ": nested macro definition is not allowed ");
      break;

    case empty_macro_declaration_err:
      /* mcr _ */
      r_error ("", line, "empty macro declaration");
      break;

    case unexpected_endmcr_err:
      /* S: .string "no macro before"
       * ednmcr */
      r_error ("unexpected ", line, "");
      break;

      /* =========================== first pass errors: */
    case exceeds_available_memory:
      printf ("%s:%-2d" RED_COLOR "error: " RESET_COLOR "Program size exceeds "
              "available memory space (4096 bytes).\n", line->file, line->num);
      break;

    case contain_non_alphanumeric_err:
      /* LABEL_L: | L!! | L-A-B-E-L! */
      r_error ("", line, " contains non-alphanumeric characters");
      break;

    case label_and_define_on_same_line_err:
      /* LABEL: .define d = 3 */
      line_to_postfix (line); /*get the fist tok again for the error msg */
      lineTok (line);
      r_error ("label ", line, " and '.define' cannot be declared on the same line");
      break;

    case expected_colon_err:
      /* LABEL .data 1,2,3 | LABEL mov r2,r3 */
      r_error ("expected ':' after ", line, "");
      break;

    case undefined_command_err:
      /* @#$DQ mov r0, r3 | 1232 */
      r_error ("undefined: ", line, "");
      break;

      /* --------------- imm */
    case label_instead_of_imm_err:
      /* mov r0, #STR | mov r0, LABEL[STR] */
      if (line->token[0] == '#') {
        move_one_char_to_prefix (line);
      }
      r_error ("expected numeric value or defined constant, but got label ", line, "");
      break;

    case exceeds_integer_bounds:
      if (line->token[0] == '#') {
        move_one_char_to_prefix (line);
      }
      r_error ("", line, " exceeds integer bounds [-(2^11-1), 2^11-1]");
      break;

    case invalid_imm_err:
      /* mov r0 #3!a |  mov r0 L[***] | .data 1,2,$ */
      r_error ("expected numeric value or defined constant, but got: ", line, "");
      break;

    case expected_imm_after_sign_err:
      /*  mov r0, #_  (relevant only for operand) */
      r_error ("expected numeric value or defined constant after ", line, "");
      break;

      /* --------------- index */
    case expected_bracket_got_nothing_err:
      /* mov r0, L[1 */
      r_error ("expected ']'", line, "");
      break;

    case expected_bracket_before_expression_err:
      /* mov r0, L[1 k] */
      r_error ("expected ']' before", line, "");
      break;

    case expected_int_before_bracket:
      /* mov r0, L[] | prn L[ ]*/
      r_error ("expected numeric value or defined constant before ", line, "");
      break;

      /* --------------- string */
    case empty_string_declaration_err:
      /* .string _ */
      r_error ("empty string declaration", line, "");
      break;

    case missing_opening_quote_err:
      /* .string a" */
      r_error ("missing opening \" character in ", line, "");
      break;

    case missing_terminating_quote_err:
      /*.string " || .string "abc   */
      r_error ("missing terminating \" character in ", line, "");
      break;

      /* --------------- operator */
    case invalid_operand_err:
      /*mov r2, $%#! */
      r_error ("", line, " invalid operand");
      break;

    case invalid_addressing_mode_err:
      r_error ("passing ", line, " is invalid addressing mode to function");
      break;

    case too_few_arguments_err:
      /* mov r2, _ | mov r2 _ | mov _ _ */
      r_error ("too few arguments in instruction ", line, "");
      break;

      /* --------------- comma */
    case expected_expression_before_comma_err:
      /* mov, r2, r3 | mov r2,,r3 | .data ,1,2 | .data 1,,2 */
      r_error ("expected expression before ", line, " token");
      break;

    case expected_comma_before_expression_err:
      /* mov r0 r3 | .data 1 2 */
      r_error ("expected ',' before ", line, "");
      break;

      /* --------------- data */
    case unexpected_comma_after_end_err:
      r_error ("unexpected ',' after end of command", line, "");
      /* .data 1,_ */
      break;

    case empty_data_declaration_err:
      /* .data _ */
      r_error ("empty data declaration", line, "");
      break;

    case expected_integer_before_comma_err:
      /* .data 1,,2 */
      r_error ("expected numeric value or defined constant before ", line, "");
      break;

      /* --------------- define & extern & entry */
    case expected_identifier_err:
      /* .define _ | .extern _ | .entry _*/
      r_error ("expected identifier after directive", line, "");
      break;

      /* define */
    case empty_define_declaration_err:
      /*.define x */
      r_error ("empty define declaration", line, "");
      break;

    case expected_equals_before_expression_err:
      /*.define x y | .define x 3 */
      r_error ("expected '=' before numeric token, but got ", line, "");
      break;

    case expected_numeric_expression_after_equal_err:
      /* .define x = _ */
      r_error ("expected numeric expression after '='", line, "");
      break;

    case invalid_numeric_expression_err:
      /* .define x = y | .define @#$#@ */
      r_error ("", line, " is not a valid numeric expression");
      break;

    default:
      break;
  }
}

void raise_second_pass_err (LineParts *line_parts, char *wanted_tok,
                            char *file_name)
{
  /* setting the wanted token in the token in order to print the error */
  char prefix[MAX_LINE_LEN], token[MAX_LINE_LEN];
  line_parts->prefix = prefix;
  line_parts->token = token;
  line_parts->postfix = line_parts->line;
  line_parts->file = file_name;

  trim_end (line_parts->postfix);
  split_line (line_parts, wanted_tok);

  raise_error (undeclared_err, line_parts);
}

/**
 * @brief Helper function to print an error message with red color.
 *
 * @param msg_before Message before the token.
 * @param line       Pointer to the line details.
 * @param msg_after  Message after the token.
 */
void r_error (char *msg_before, LineParts *line, char *msg_after)
{
  line_msg ("error", RED_COLOR, msg_before, line, msg_after);
}

/**
 * @brief Helper function to print a warning message with yellow color.
 *
 * @param msg_before Message before the token.
 * @param line       Pointer to the line details.
 * @param msg_after  Message after the token.
 */
void r_warning (char *msg_before, LineParts *line, char *msg_after)
{
  line_msg ("warning", YEL_COLOR, msg_before, line, msg_after);
}

/**
 * @brief Helper function to print a line message with specified type, color,
 * message before the token, line details, and message after the token.
 * print in a gcc style. for example:
 *
 * txt.am:3  error: 'EXAMPLE' undeclared
 3   | mov r0, EXAMPLE
     |         ^~~~~~~
 *
 * @param type       Type of the message (error/warning).
 * @param color      Color of the message.
 * @param msg_before Message before the token.
 * @param line       Pointer to the line details.
 * @param msg_after  Message after the token.
 */
void line_msg (char *type, char *color, char *msg_before, LineParts *line,
               char *msg_after)
{
  int i;
  char c;
  /* Print file and line number, error or warning type (fileNum:i error:) */
  printf ("\n%s:%-2d %s%s: " RESET_COLOR, line->file, line->num, color, type);

  /* Print message context, token, and additional message */
  if (IS_EMPTY(line->token)) {
    if (IS_EMPTY(msg_before)) {
      printf ("%s\n", msg_after);
    }
    else {
      printf ("%s\n", msg_before);
    }
  }
  else {
    printf ("%s" BOLD "'%s'" REG "%s\n", msg_before, line->token, msg_after);
  }


  /* print line number and the line with the token bolded in color
   i | line with error cause bolted in color */
  printf (" %-3d | %s%s%s" RESET_COLOR "%s\n",
          line->num, line->prefix, color, line->token, line->postfix);

  /* print an arrow pointing to the location of the token in the line
       |           ^~~~~~~~~                 */
  printf (" %-3s |", " ");
  for (i = 0; i < (int) strlen (line->prefix); i++) {
    c = line->prefix[i] == '\t' ? '\t' : ' ';
    printf ("%c", c);
  }
  printf (" %s^", color);
  for (i = 0; i < (int) strlen (line->token) - 1; i++) {
    printf ("%s~", color);
  }
  printf (RESET_COLOR "\n");
}