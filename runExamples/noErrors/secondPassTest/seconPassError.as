; *************** second pass test ***************

;--------------- resolved use
mov USE1, r1
.extern USE1

mov USE2, r2
USE2: .string "2"

;--------------- resolved entry & use
mov USE3, r3
.entry USE3
USE3: .data 3

.entry USE4
mov USE4, r4
USE4: hlt

;--------------- resolved entry
.entry ENT5
ENT5: .string "hi"

.entry ENT6
ENT6: cmp #1, #2