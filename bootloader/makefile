NASM := nasm

BINDIR = ./bin

# Build flags
NASM_FLAGS += -f bin

# Make build directory
$(shell mkdir -p $(BINDIR))

.PHONY: clean build

clean:
	rm -f $(BINDIR)/bootloader.bin

build:
	$(NASM) $(NASM_FLAGS) boot.asm -o $(BINDIR)/bootloader.bin

