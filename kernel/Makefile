OBJDIR = $(BUILD_DIR)/kernel

OBJS = \
		$(OBJDIR)/main.o\
		$(OBJDIR)/intr.o\
		$(OBJDIR)/init.o\
		$(OBJDIR)/proc.o\
		$(OBJDIR)/lock.o

TARGET_FILE = $(BUILD_DIR)/kernel.o

$(TARGET_FILE): $(OBJDIR) $(OBJS) 
	$(LD) $(LDFLAGS) -o $@ -r $(OBJS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	mkdir $(OBJDIR)