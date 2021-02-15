OBJDIR = $(BUILD_DIR)/fs

OBJS = \
		$(OBJDIR)/file.o\
		$(OBJDIR)/inode.o

TARGET_FILE = $(BUILD_DIR)/fs.o

$(TARGET_FILE): $(OBJDIR) $(OBJS) 
	$(LD) $(LDFLAGS) -o $@ -r $(OBJS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	mkdir $(OBJDIR)