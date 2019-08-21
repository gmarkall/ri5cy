set(TESTS 00-mvcop
          01-cmov
          02-li
          03-bop
          04-ins-ext
          05-packed-arith-add
          05-packed-arith-mul
          05-packed-arith-sub
          06-packed-shift
          07-packed-shifti
          08-load
          10-load-h
          11-load-b
          12-store-w
          13-store-h
          14-store-b
          15-gather-b
          16-scatter-b
          17-gather-h
          18-scatter-h
          19-random
          20-add2-mp
          21-add3-mp
          22-slli-mp
          23-sll-mp
          24-srl-mp
          25-srli-mp
          26-acc2-mp
          27-acc1-mp
          28-sub2-mp
          29-sub3-mp
          30-mac-mp
          31-equ-mp
          32-ltu-mp
          33-gtu-mp
          34-packed-arith-mul-h
          35-packed-arith-clmul-h
          36-packed-arith-clmul-l
          37-mclmul
          38-aes-sub
          39-aes-mix
          40-sbox
          41-sha3)

foreach (_TEST ${TESTS})
  set(_SOURCE ${_TEST}.S)
  set(_OBJ ${_TEST}.o)
  set(_PROG ${_TEST}.exe)
  set(_BIN ${_TEST}.bin)
  add_custom_command(OUTPUT ${_OBJ}
                     COMMAND riscv32-unknown-elf-as
		     ARGS -march=rv32ix -o ${_OBJ} ${_SOURCE}
		     DEPENDS ${_SOURCE}
		     COMMENT "Assembling ${_TEST}")

  add_custom_command(OUTPUT ${_PROG}
                     COMMAND riscv32-unknown-elf-ld
		     ARGS -T ${CMAKE_CURRENT_SOURCE_DIR}/test.ld -o ${_PROG} ${_OBJ}
		     DEPENDS ${_OBJ}
		     COMMENT "Linking ${_TEST}")

  add_custom_command(OUTPUT ${_BIN}
                     COMMAND riscv32-unknown-elf-objcopy
		     ARGS --dump-section .text=${_BIN} ${_PROG}
		     DEPENDS ${_PROG}
		     COMMENT "Dumping text section for ${_TEST}")

  add_custom_target(${_TEST}-target ALL DEPENDS ${_BIN})

  add_test(NAME ${_TEST}
           COMMAND ../testbench ${_BIN})
endforeach()
