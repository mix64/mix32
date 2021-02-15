OBJDIR = $(BUILD_DIR)/mm

OBJS = \
		$(OBJDIR)/pmm.o\
		$(OBJDIR)/vmm.o
		
TARGET_FILE = $(BUILD_DIR)/mm.o

$(TARGET_FILE): $(OBJDIR) $(OBJS) 
	$(LD) $(LDFLAGS) -o $@ -r $(OBJS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	mkdir $(OBJDIR)