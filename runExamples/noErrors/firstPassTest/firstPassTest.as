; *************** first pass test ***************

STR: .string "hello world! 123"
ENT: .string ""

.define def = 1

INT: .data +100,-100,11, def

.extern EXT
.entry ENT

;----- imm as src
CODE: mov #-01, INT
add #def, STR[2]
sub EXT, r3

cmp r0, #4

lea INT[def], STR

not CODE[def]
clr EXT[ 3 ]
inc r4
dec STR

jmp r5
bne INT
jsr EXT

red CODE[15]

prn #-9

rts
hlt