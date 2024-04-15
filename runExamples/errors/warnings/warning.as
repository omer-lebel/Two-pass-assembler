; ======================== warnings ========================

;--------- inaccessible_data
.data 1,2,3
.string "a"

;--------- ignored_label
L1: .extern EXT
L2: .entry ENT

;--------- duplicate_declaration_warning
.extern EXT
.entry ENT

.define d=1
.define d=1



ENT: .data 5
