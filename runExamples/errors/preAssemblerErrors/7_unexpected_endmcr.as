; ============== test 7 =================
; unexpected (no matching mcr)
; output: error msg, 0 files

.define d=4

    add r2, r3
    mov r2, r3
endmcr

prn #d