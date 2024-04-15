; ============== test 4 =================
; redeclaration of macro
; output: error msg, 0 files

prn #-2

mcr MAC
    sub r2, r3
endmcr

clr r4
.define x=5

mcr MAC
    .data 1,x,x,x
endmcr