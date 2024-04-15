; ============== test 5 =================
; extraneous text after macro name
; output: error msg, 0 files

clr r4
.define x=5

mcr MAC //extra-text
    .data 1,x,x,x
endmcr

inc r4