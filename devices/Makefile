OBJDIR = $(BUILD_DIR)/devices

OBJS = \
		$(OBJDIR)/console.o\
		$(OBJDIR)/disk.o
TARGET_FILE = $(BUILD_DIR)/devices.o


$(TARGET_FILE): $(OBJDIR) $(OBJS) 
	$(LD) $(LDFLAGS) -o $@ -r $(OBJS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	mkdir $(OBJDIR)