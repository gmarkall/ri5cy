_start:
        li a0, 0x746575b7 # a0 = "test"
        li a1, 1
        li a2, 2
        li a3, 3
        li a4, 4
        .insn i 0x0b, 0, x0, a1, 0 # upper x0, a1
        .insn i 0x0b, 0, a5, a1, 0 # upper a5, a1
        .insn i 0x0b, 1, a5, a1, 0 # lower a5, a1
        .insn i 0x0b, 2, a5, a1, 0 # leet a5, a1
        .insn i 0x0b, 3, a5, a1, 0 # rot13 a5, a1
exit:
        li a0, 0
        li a1, 0
        li a2, 0
        li a7, 93
        ecall
