test_targets := $(filter-out $(KERNEL_DIR)/entry.o,$(targets))
test_objs := $(patsubst %.o,$(BUILD_DIR)/%.o,$(test_targets))

test: $(TEST_TARGET_DIR)/test_string

test_string_objs := $(test_objs)
test_string_objs += $(BUILD_DIR)/test/utils/string.o

$(TEST_TARGET_DIR)/test_string: $(test_string_objs) $(LDSCRIPT)
	$(hide)$(LD) $(LDFLAGS) -o $@ -T $(LDSCRIPT) $(test_string_objs)
	@echo "	[LD] $@"