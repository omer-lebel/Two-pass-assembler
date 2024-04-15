; ----------- test 10 -----------
; line length is more than 80 characters
; output: *.am file, *.ob file
;         *error msg about line 6,12

mov r2, r3                                                                       extra text after 80+1 chars
LABEL: cmp r7, r4
       add LABEL, r2

mcr MACRO
    sub r2, r3                                                                      extra text after 80+1 chars
endmcr

jmp LABEL