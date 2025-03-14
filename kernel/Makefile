interrupt_targets := idt.o interrupt.o vecs.o syscall.o

device_targets := pci.o sys_clk.o keyboard.o

task_targets := kthread.o switch.o task.o sync.o tss.o uproc.o pid.o

utils_targets := print.o string.o bitmap.o list_head.o queue.o

memory_targets := page.o memory.o

kernel_targets := entry.o

kernel_targets += $(patsubst %.o, interrupt/%.o,$(interrupt_targets))
kernel_targets += $(patsubst %.o, device/%.o,$(device_targets))
kernel_targets += $(patsubst %.o, utils/%.o,$(utils_targets))
kernel_targets += $(patsubst %.o, memory/%.o,$(memory_targets))
kernel_targets += $(patsubst %.o, task/%.o,$(task_targets))
