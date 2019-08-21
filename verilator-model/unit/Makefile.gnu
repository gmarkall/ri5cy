
INPUTS      ?= $(shell find -name "*.S")
OUTPUTS     ?= $(INPUTS:%.S=%.exe) $(INPUTS:%.S=%.bin)

AS           = riscv32-unknown-elf-as
LD           = riscv32-unknown-elf-ld
OBJDUMP      = riscv32-unknown-elf-objdump
OBJCOPY      = riscv32-unknown-elf-objcopy

all: $(OUTPUTS)

%.o : %.S
	$(AS) -march=rv32ix -o $@ $<

%.bin : %.exe
	$(OBJCOPY) --dump-section .text=$@ $<

%.exe : %.o
	$(LD) $< -o $@ -T test.ld

.PHONY: clean
clean:
	rm -rf $(OUTPUTS)
