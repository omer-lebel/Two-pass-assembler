; ============== test 6 =================
; extraneous text after endmcr
; output: error msg, 0 files

clr r4
.define x=5

mcr MAC
    .data 1,x,x,x
endmcr //extra-text

inc r4