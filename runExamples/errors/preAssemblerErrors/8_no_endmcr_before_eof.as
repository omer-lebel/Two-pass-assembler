; ============== test 8 =================
; reached EOF in the middle of macro definition. Expected 'endmcr'
; output: error msg, 0 files


mcr MACRO1
    cmp r3, r4

dec r5