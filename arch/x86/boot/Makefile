OBJDIR = $(BUILD_DIR)/boot

$(BUILD_DIR)/bootblock: $(OBJDIR) 
	$(CC) $(CFLAGS) -o $(OBJDIR)/bootmain.o -c bootmain.c
	$(CC) $(CFLAGS) -o $(OBJDIR)/bootasm.o -c bootasm.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 -o $(OBJDIR)/bootblock.o $(OBJDIR)/bootasm.o $(OBJDIR)/bootmain.o
	$(OBJCOPY) -S -O binary -j .text $(OBJDIR)/bootblock.o $(BUILD_DIR)/bootblock
	./sign.pl $(BUILD_DIR)/bootblock
	
$(OBJDIR):
	mkdir $(OBJDIR)
