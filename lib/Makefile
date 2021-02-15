OBJDIR = $(BUILD_DIR)/lib

OBJS = \
		$(OBJDIR)/string.o
TARGET_FILE = $(BUILD_DIR)/lib.o


$(TARGET_FILE): $(OBJDIR) $(OBJS) 
	$(LD) $(LDFLAGS) -o $@ -r $(OBJS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	mkdir $(OBJDIR)