OBJDIR = $(BUILD_DIR)/arch

ifeq ($(TARGET), X86)
OBJS = \
		$(OBJDIR)/x86-console.o\
		$(OBJDIR)/x86-intr.o\
		$(OBJDIR)/x86-mm.o\
		$(OBJDIR)/x86-initcode.o\
		$(OBJDIR)/x86-proc.o\
		$(OBJDIR)/x86-trap.o\
		$(OBJDIR)/x86-trapasm.o\
		$(OBJDIR)/x86-vectors.o\
		$(OBJDIR)/x86-kbd.o\
		$(OBJDIR)/x86-disk.o\
		$(OBJDIR)/x86-timer.o
BOOT_BLOCK = $(BUILD_DIR)/x86-bootblock
TARGET_FILE = $(BUILD_DIR)/x86.o
endif

ifeq ($(TARGET), RV32)
endif

$(TARGET_FILE): $(OBJDIR) $(OBJS) $(BOOT_BLOCK)
	$(LD) $(LDFLAGS) -o $@ -r $(OBJS)

# x86
$(OBJDIR)/x86-%.o: x86/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/x86-%.o: x86/%.S
	$(CC) $(CFLAGS) -o $@ -c $<

x86/vectors.S:
	./x86/vectors.pl > ./x86/vectors.S

$(BUILD_DIR)/x86-bootblock: $(OBJDIR)
	$(CC) $(CFLAGS) -o $(OBJDIR)/bootasm.o -c x86/boot/bootasm.S
	$(CC) $(CFLAGS) -o $(OBJDIR)/bootmain.o -c x86/boot/bootmain.c
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 -o $(OBJDIR)/bootblock.o $(OBJDIR)/bootasm.o $(OBJDIR)/bootmain.o
	$(OBJCOPY) -S -O binary -j .text $(OBJDIR)/bootblock.o $@
	./x86/boot/sign.pl $@



# rv32
$(OBJDIR)/rv32-%.o: rv32/%.c
	$(CC) $(CFLAGS) -o $@ -c rv32/$<

# $(BUILD_DIR)/rv32-bootblock:


$(OBJDIR):
	mkdir $(OBJDIR)