boot_targets := bootsec.o loader.o boot_gdt.o

interrupt_targets := exception.o idt.o interrupt.o syscall.o vecs.o

include $(KERNEL_DIR)/device/Makefile

task_targets := task.o pid.o sync.o kthread.o switch.o tss.o uproc.o

fs_targets := filesystem.o inode.o

utils_targets := print.o compiler.o bitmap.o

include $(KERNEL_DIR)/memory/Makefile

kernel_targets := entry.o main.o

kernel_targets += $(patsubst %.o, boot/%.o,$(boot_targets))
kernel_targets += $(patsubst %.o, interrupt/%.o,$(interrupt_targets))
kernel_targets += $(patsubst %.o, device/%.o,$(device_targets))
kernel_targets += $(patsubst %.o, utils/%.o,$(utils_targets))
kernel_targets += $(patsubst %.o, memory/%.o,$(memory_targets))
kernel_targets += $(patsubst %.o, task/%.o,$(task_targets))
kernel_targets += $(patsubst %.o, filesystem/%.o,$(fs_targets))