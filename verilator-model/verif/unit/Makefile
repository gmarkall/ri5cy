
OUT_DIR     ?= $(PWD)/unit

INPUTS      ?= $(shell find -name "*.S")
#OUTPUTS     ?= $(addprefix $(OUT_DIR)/, $(INPUTS:%.S=%.hex)) $(addprefix $(OUT_DIR)/, $(INPUTS:%.S=%.exe))
OUTPUTS     ?= $(addprefix $(OUT_DIR)/, $(INPUTS:%.S=%.exe))

AS           = riscv32-unknown-elf-as
LD           = riscv32-unknown-elf-ld
OBJDUMP      = riscv32-unknown-elf-objdump

all: $(OUTPUTS)

$(OUT_DIR)/%.o : %.S
	@mkdir -p $(OUT_DIR)
	$(AS) -march=rv32ix -o $@ $<

$(OUT_DIR)/%.exe : $(OUT_DIR)/%.o
	$(LD) $< -o $@ -T test.ld

$(OUT_DIR)/%.hex : $(OUT_DIR)/%.o
	$(OBJDUMP) -D -j.text $< | grep -P ":\t" > $@
	sed -i 's/     .*$///' $@
	sed -i 's/^.*:\t//' $@

clean:
	rm -rf $(OUTPUTS)
