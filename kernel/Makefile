interrupt_targets := idt.o interrupt.o vecs.o

device_targets := pci.o sys_clk.o vga.o

task_targets := thread.o switch.o task.o sync.o

utils_targets := print.o string.o bitmap.o list_head.o

memory_targets := page.o memory.o

kernel_targets := entry.o types.o log.o

kernel_targets += $(patsubst %.o, interrupt/%.o,$(interrupt_targets))
kernel_targets += $(patsubst %.o, device/%.o,$(device_targets))
kernel_targets += $(patsubst %.o, utils/%.o,$(utils_targets))
kernel_targets += $(patsubst %.o, memory/%.o,$(memory_targets))
kernel_targets += $(patsubst %.o, task/%.o,$(task_targets))
