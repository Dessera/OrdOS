interrupt_targets := vecs.o idt.o
device_targets := pci.o sys_clk.o vga.o
utils_targets := print.o mm.o string.o bitmap.o list_head.o
memory_targets := page.o

kernel_targets := entry.o assert.o

kernel_targets += $(patsubst %.o, interrupt/%.o,$(interrupt_targets))
kernel_targets += $(patsubst %.o, device/%.o,$(device_targets))
kernel_targets += $(patsubst %.o, utils/%.o,$(utils_targets))
kernel_targets += $(patsubst %.o, memory/%.o,$(memory_targets))
