#!/bin/bash

riscv32-unknown-elf-as -march=rv32im examples.s -o examples.o
riscv32-unknown-elf-objdump -dr examples.o
