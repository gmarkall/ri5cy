
OUT_DIR     ?= unit

INPUTS      ?= $(shell find -name "*.S")
OUTPUTS     ?= $(addprefix $(OUT_DIR)/, $(INPUTS:%.S=%.exe)) $(addprefix $(OUT_DIR)/, $(INPUTS:%.S=%.bin))

AS           = riscv32-unknown-elf-as
LD           = riscv32-unknown-elf-ld
OBJDUMP      = riscv32-unknown-elf-objdump
OBJCOPY      = riscv32-unknown-elf-objcopy

all: $(OUTPUTS)

$(OUT_DIR)/%.o : %.S
	@mkdir -p $(OUT_DIR)
	$(AS) -march=rv32ix -o $@ $<

$(OUT_DIR)/%.bin : $(OUT_DIR)/%.exe
	$(OBJCOPY) --dump-section .text=$@ $<

$(OUT_DIR)/%.exe : $(OUT_DIR)/%.o
	$(LD) $< -o $@ -T test.ld

.PHONY: clean
clean:
	rm -rf $(OUTPUTS)
