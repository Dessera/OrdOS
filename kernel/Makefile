boot_targets := bootsec.o loader.o boot_gdt.o

interrupt_targets := exception.o idt.o interrupt.o syscall.o vecs.o

device_targets := pci.o

# task_targets := kthread.o switch.o task.o sync.o tss.o uproc.o pid.o

utils_targets := print.o string.o compiler.o

memory_targets := memory.o gdt.o e820.o bootmem.o page.o

kernel_targets := entry.o main.o

kernel_targets += $(patsubst %.o, boot/%.o,$(boot_targets))
kernel_targets += $(patsubst %.o, interrupt/%.o,$(interrupt_targets))
kernel_targets += $(patsubst %.o, device/%.o,$(device_targets))
kernel_targets += $(patsubst %.o, utils/%.o,$(utils_targets))
kernel_targets += $(patsubst %.o, memory/%.o,$(memory_targets))
# kernel_targets += $(patsubst %.o, task/%.o,$(task_targets))
