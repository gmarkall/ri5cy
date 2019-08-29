_start:
        # Load some constants and do some arithmetic
        li a0, 0
        li a1, 0x74657374
        li a2, 2
        li a3, 3
        li a4, 4
        li a5, 5
        li a6, 6
        li a7, 7
        .insn i 0x0b, 0, a0, a1, 0 # upper a0, a1
        .insn i 0x0b, 0, a2, a7, 0 # upper a5, a1
        .insn i 0x0b, 1, a3, a4, 0 # lower a5, a1
        .insn i 0x0b, 2, a4, a5, 0 # leet a5, a1
        .insn i 0x0b, 3, a5, a1, 0 # rot13 a5, a1
        add t0, a1, a2
        add t1, a3, a4
        add t2, a4, a5
        add t0, t1, t2
exit:
        # Similar to exit(0) in C.
        li a0, 0
        li a1, 0
        li a2, 0
        li a7, 93
        ecall
