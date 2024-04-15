; *************** pre assembler test ***************
; check that macro expend works

.define x=1
STR: .string "pre assembler test"
L:  mov r2, r3

mcr my_mcr
    ;this ia a macro
    cmp r3, #x
    bne L
endmcr

    lea STR, STR
    prn #-2
    my_mcr
    inc L