; ============== test 9 =================
; nested macro definition is not allowed
; output: error msg, 0 files

mov r0, r2

mcr MACRO1
    cmp r3, r4
    mcr MACRO2
        add r4, 45
    endmcr
endmcr

clr r4