interrupt_targets := vecs.o idt.o
device_targets := pci.o sys_clk.o vga.o
utils_targets := print.o mm.o

kernel_targets := entry.o assert.o

kernel_targets += $(patsubst %.o, interrupt/%.o,$(interrupt_targets))
kernel_targets += $(patsubst %.o, device/%.o,$(device_targets))
kernel_targets += $(patsubst %.o, utils/%.o,$(utils_targets))
