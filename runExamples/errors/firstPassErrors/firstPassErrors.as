L: mov r0, r1
STR: .string "test"
DATA: .data 1,2,3

; ======================== first pass errors ========================  */

;--------- extraneous_text_err,
.extern X 123
prn EXT xxx

;--------- starts_with_non_alphabetic_err,
1L: .data 2,3
.entry $EN
.define %x = 3

;--------- reserved_keyword_used_err,
mov: .string "a"
.extern r2
.define endmcr = 2


;--------- redeclaration_err,
.define d=3

S1: .string "b"
S1: .string "b"

.extern DATA

;--------- undeclared_err,
D2: .data 1,z,2
mov S2[z], r0


;--------- contain_non_alphanumeric_err,
LABEL_L: hlt
L-A-B-E-L!: rts

;--------- label_and_define_on_same_line_err,
L3: .define d2 = 5

;expected_colon_err,
LABEL add r2,r3

;undefined_command_err,
@#$DQ mov r0, r3
LABEL:


; ======================== imm ========================  */

;--------- label_instead_of_imm_err,
mov r0, #STR
add r0, LABEL[STR]

;--------- exceeds_integer_bounds,
cmp r0, #500000000
.define d5 = -999999
dec EXT[2222222]

;--------- invalid_imm_err,
clr #3!a
clr L[***]
.data 1,2,$

;--------- expected_imm_after_sign_err,
mov r0, #


; ======================== index ======================== */

;--------- expected_bracket_got_nothing_err,
inc L[1

;--------- expected_bracket_before_expression_err,
mov r0, L[1 k]

;--------- expected_int_before_bracket,
prn L[]

; ======================== string ======================== */

;--------- empty_string_declaration_err,
.string

;--------- missing_opening_quote_err,
.string a"

;--------- missing_terminating_quote_err,
.string "abc

; ======================== operator ======================== */

;--------- invalid_operand_err,
mov r2, $%#!

;--------- invalid_addressing_mode_err,
jmp #2

;--------- too_few_arguments_err,
jmp
add r2

;--------- expected_expression_before_comma_err,
mov, r2, r3

;--------- expected_comma_before_expression_err,
cmp r0 r3
.data 1 2

; ======================== data ======================== */

;--------- unexpected_comma_after_end_err,
.data 1,
not r0,

;--------- empty_data_declaration_err,
.data

;--------- expected_integer_before_comma_err,
.data 1,,2

; ======================== define & extern & entry ======================== */

;--------- expected_identifier_err,
.entry
.extern

; ======================== define
;--------- empty_define_declaration_err,
.define d6

;--------- expected_equals_before_expression_err,
.define d7 7

;--------- expected_numeric_expression_after_equal_err,
.define x =

;--------- invalid_numeric_expression_err
.define @#$#@
.define x = y