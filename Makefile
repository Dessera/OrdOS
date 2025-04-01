VERSION = 0.0.0
NAME = OrdOS
NAMEFILE = $(shell echo $(NAME) | tr 'A-Z' 'a-z')

BUILD_DIR = build
KERNEL_DIR = kernel
INCLUDE_DIR = include
TEST_DIR = test
USER_DIR = user
LIB_DIR = lib
SCRIPTS_DIR = scripts

DISK_NAME = disk.img
DISK_SIZE = 80M
DISK_PARTITIONS = disk.partitions
DISK_IMG = $(BUILD_DIR)/$(DISK_NAME)

# lower case
TARGET = $(BUILD_DIR)/$(NAMEFILE)
TARGET_SYMS = $(BUILD_DIR)/$(NAMEFILE).syms
TARGET_DEBUG = $(BUILD_DIR)/$(NAMEFILE).debug

DEBUG = 1
LOGLEVEL = 4		# 0: none, 1: error, 2: warning, 3: info, 4: debug 5: trace
BUILD_TRACE =

TESTER =
SCRIPT =
SCRIPT_ARGS =

STDFLAGS = -std=c23

CFLAGS = -Wall -Werror -W -Wstrict-prototypes -Wmissing-prototypes						\
					-mno-sse -fno-builtin -fno-pie -fno-pic -fno-stack-protector 				\
					-nostdinc -nostdlib -m32 -I$(INCLUDE_DIR) $(STDFLAGS)								\
					-D KVERSION=$(VERSION) -D KNAME=$(NAME) -D LOGLEVEL=$(LOGLEVEL)

ifneq ($(DEBUG),)
CFLAGS += -g -O0 -D DEBUG
else
CFLAGS += -Os
endif

ASFLAGS = $(CFLAGS)

LDSCRIPT_TEMPLATE = kernel.ld.template
LDSCRIPT = $(BUILD_DIR)/kernel.ld
LDFLAGS = -z noexecstack --no-warn-rwx-segments

CFLAGS_SCRIPTS := $(STDFLAGS) -I$(INCLUDE_DIR)/kernel/config -include $(INCLUDE_DIR)/lib/common.h

targets :=

# if BUILD_TRACE is set, set hide to empty, otherwise set it to @
ifeq ($(BUILD_TRACE),)
hide=@
else
hide=
endif

.PHONY: all clean qemu rebuild

all: $(TARGET) $(TARGET_DEBUG) $(DISK_IMG)

# 	-------------------- LIB ----------------------
include $(LIB_DIR)/Makefile

targets += $(patsubst %.o, $(LIB_DIR)/%.o,$(lib_targets))
# 	-----------------------------------------------

# 	-------------------- KERNEL -------------------
include $(KERNEL_DIR)/Makefile

targets += $(patsubst %.o, $(KERNEL_DIR)/%.o,$(kernel_targets))
# 	-----------------------------------------------

# 	-------------------- USER -------------------
include $(USER_DIR)/Makefile

# targets += $(patsubst %.o, $(USER_DIR)/%.o,$(user_targets))

$(DISK_IMG): $(DISK_PARTITIONS)
	@mkdir -p $(BUILD_DIR)
	$(hide)qemu-img create $(DISK_IMG) $(DISK_SIZE)
	$(hide)cat $(DISK_PARTITIONS) | sfdisk $(DISK_IMG)
# 	-----------------------------------------------

# 	-------------------- FINAL --------------------
final_objs := $(patsubst %.o,$(BUILD_DIR)/%.o,$(targets))

$(LDSCRIPT): $(LDSCRIPT_TEMPLATE)
	@mkdir -p $(BUILD_DIR)
	$(hide)$(CC) -E -P -I$(INCLUDE_DIR) - < $< > $@
	@echo "	[CP] $(LDSCRIPT)"

$(TARGET): $(final_objs) $(LDSCRIPT)
	$(hide)$(LD) $(LDFLAGS) -o $@ -T $(LDSCRIPT) $(final_objs) -Map $(TARGET_SYMS)
	@echo "	[LD] $(TARGET)"

$(TARGET_DEBUG): $(final_objs) $(LDSCRIPT)
	$(hide)$(LD) $(LDFLAGS) -o $@ -T $(LDSCRIPT) --oformat elf32-i386 $(final_objs) -Map $(TARGET_SYMS)
	@echo "	[LD] $(TARGET_DEBUG)"
# 	-----------------------------------------------

# 	-------------------- UTILS --------------------
qemu_debug: $(TARGET) $(TARGET_DEBUG) $(DISK_IMG)
	qemu-system-i386 -s -S -drive format=raw,file=$(TARGET) -drive format=raw,file=$(DISK_IMG)

qemu: $(TARGET) $(DISK_IMG)
	qemu-system-i386 -drive format=raw,file=$(TARGET) -drive format=raw,file=$(DISK_IMG)

clean:
	$(hide)rm -rf $(BUILD_DIR)
	@echo "	[RM] $(BUILD_DIR)"

rebuild: clean all
# 	-----------------------------------------------

# 	-------------------- TESTER -------------------
test_targets := $(filter-out $(KERNEL_DIR)/main.o,$(targets))
test_targets += $(TEST_DIR)/tester.o
test_objs := $(patsubst %.o,$(BUILD_DIR)/%.o,$(test_targets))

include $(TEST_DIR)/Makefile

test: $(patsubst %.o, $(BUILD_DIR)/$(TEST_DIR)/%,$(test_entries))

# makefile remove them for unknown reason (at least idk)
# use this to prevent it
test_entry_objs := $(patsubst %.o, $(BUILD_DIR)/$(TEST_DIR)/%.o,$(test_entries))
.PRECIOUS: $(test_entry_objs)

ifneq ($(TESTER),)
qemu_test: $(BUILD_DIR)/$(TEST_DIR)/$(TESTER)
	qemu-system-i386 -drive format=raw,file=$(BUILD_DIR)/$(TEST_DIR)/$(TESTER)
endif
# 	-----------------------------------------------

# 	-------------------- SCRIPTS ------------------
include $(SCRIPTS_DIR)/Makefile

scripts_objs := $(patsubst %.o,$(BUILD_DIR)/$(SCRIPTS_DIR)/%.o,$(scripts_entries))

scripts: $(patsubst %.o, $(BUILD_DIR)/$(SCRIPTS_DIR)/%,$(scripts_entries))

ifneq ($(SCRIPT),)
run_script: $(BUILD_DIR)/$(SCRIPTS_DIR)/$(SCRIPT)
	@$(BUILD_DIR)/$(SCRIPTS_DIR)/$(SCRIPT) $(SCRIPT_ARGS)
endif

$(BUILD_DIR)/$(SCRIPTS_DIR)/%: $(SCRIPTS_DIR)/%.c
	@mkdir -p $(dir $@)
	$(hide)$(CC) $(CFLAGS_SCRIPTS) $< -o $@
	@echo "	[SCRIPT] $<"
# 	-----------------------------------------------

# 	-------------------- RULES --------------------
$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(hide)$(CC) $(ASFLAGS) -c $< -o $@
	@echo "	[AS] $<"

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(hide)$(CC) $(CFLAGS) -c $< -o $@
	@echo "	[CC] $<"

$(BUILD_DIR)/$(TEST_DIR)/%: $(BUILD_DIR)/$(TEST_DIR)/%.o $(LDSCRIPT) $(test_objs)
	$(hide)$(LD) $(LDFLAGS) -o $@ -T $(LDSCRIPT) $(test_objs) $<
	@echo "	[TESTER] $@"
# 	-----------------------------------------------