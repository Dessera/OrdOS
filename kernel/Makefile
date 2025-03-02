interrupt_targets := vecs.o idt.o
device_targets := pci.o

kernel_targets := entry.o utils/print.o utils/mm.o

kernel_targets += $(patsubst %.o, interrupt/%.o,$(interrupt_targets))
kernel_targets += $(patsubst %.o, device/%.o,$(device_targets))
