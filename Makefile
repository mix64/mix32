export TARGET = X86
SUBDIRS = arch devices fs kernel lib mm
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -fno-asynchronous-unwind-tables 
CFLAGS += -O2 -Wall -MD -ggdb -Werror -fno-omit-frame-pointer -nostdinc -fno-pie -no-pie 
CFLAGS += -I $(abspath .)/include

ifeq ($(TARGET), X86)
export CC = gcc
export CFLAGS += -m32
export LD = ld
export LDFLAGS = -m elf_i386
export ASFLAGS = -m32 -gdwarf-2 -Wa,-divide
export OBJCOPY = objcopy
export OBJDUMP = objdump
IMGFILE = mix32-x86.img
endif

ifeq ($(TARGET), RV32)
export CC = riscv32-unknown-elf-gcc
export LD = riscv32-unknown-elf-ld
export LDFLAGS = -m elf32lriscv
export OBJCOPY = riscv32-unknown-elf-objcopy
export OBJDUMP = riscv32-unknown-elf-objdump
IMGFILE = mix32-rv32.img
endif

export BUILD_DIR = $(abspath .)/build

$(IMGFILE): $(SUBDIRS)
	$(LD) $(LDFLAGS) -T kernel.ls -o $(BUILD_DIR)/kernel.elf $(shell find $(BUILD_DIR)/*.o)
	$(OBJDUMP) -S $(BUILD_DIR)/kernel.elf > $(BUILD_DIR)/kernel.asm
	./mkfs

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	make -e -C $@ $(MAKECMDGOALS)

form:
	find -type f -name "*.[ch]" | xargs clang-format -i