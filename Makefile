VERSION = 0.0.0
NAME = KDemo
NAMEFILE = $(shell echo $(NAME) | tr 'A-Z' 'a-z')

BUILD_DIR = build
BOOT_DIR = boot
KERNEL_DIR = kernel
INCLUDE_DIR = include
TEST_DIR = test

# lower case
TARGET = $(BUILD_DIR)/$(NAMEFILE)
TEST_TARGET_DIR = $(BUILD_DIR)/$(TEST_DIR)

DEBUG = 1
TRACE =

INCFLAGS = -I$(INCLUDE_DIR)

CFLAGS = -Wall -Werror -W -Wstrict-prototypes -Wmissing-prototypes					\
					-fno-builtin -fno-pie -fno-pic -fno-stack-protector -nostdinc			\
					-nostdlib -m32 $(INCFLAGS) -D KVERSION=$(VERSION) -D KNAME=$(NAME)

ifneq ($(DEBUG),)
CFLAGS += -g -O0 -D DEBUG
else
CFLAGS += -Os
endif

ASFLAGS = $(CFLAGS) -D __ASM__

LDSCRIPT_TEMPLATE = kernel.ld.template
LDSCRIPT = $(BUILD_DIR)/kernel.ld
LDFLAGS = -z noexecstack

targets :=

# if TRACE is set, set hide to empty, otherwise set it to @
ifeq ($(TRACE),)
hide=@
else
hide=
endif

.PHONY: all clean qemu rebuild

all: $(TARGET)

clean:
	$(hide)rm -rf $(BUILD_DIR)
	@echo "	[RM] $(BUILD_DIR)"


include Makefile.rules


# 	-------------------- BOOT ---------------------
include boot/Makefile

targets += $(patsubst %.o, $(BOOT_DIR)/%.o,$(boot_targets))
# 	-----------------------------------------------

# 	-------------------- KERNEL -------------------
include kernel/Makefile

targets += $(patsubst %.o, $(KERNEL_DIR)/%.o,$(kernel_targets))
# 	-----------------------------------------------

# 	-------------------- FINAL --------------------
final_objs := $(patsubst %.o,$(BUILD_DIR)/%.o,$(targets))

$(LDSCRIPT): $(LDSCRIPT_TEMPLATE)
	@mkdir -p $(BUILD_DIR)
	$(hide)gcc -E -P $(INCFLAGS) - < $< > $@
	@echo "	[CP] $(LDSCRIPT)"

$(TARGET): $(final_objs) $(LDSCRIPT)
	$(hide)$(LD) $(LDFLAGS) -o $@ -T $(LDSCRIPT) $(final_objs)
	@echo "	[LD] $(TARGET)"
# 	-----------------------------------------------

# 	-------------------- UTILS --------------------
ifneq ($(DEBUG),)
qemu: $(TARGET)
	qemu-system-i386 -s -S -drive format=raw,file=$(TARGET)
else
qemu: $(TARGET)
	qemu-system-i386 -drive format=raw,file=$(TARGET)
endif

rebuild: clean all
# 	-----------------------------------------------

# 	-------------------- TESTER -------------------
include test/Makefile
# 	-----------------------------------------------
