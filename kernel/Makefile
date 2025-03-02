interrupt_targets := vecs.o idt.o

kernel_targets := entry.o utils/print.o utils/mm.o

kernel_targets += $(patsubst %.o, interrupt/%.o,$(interrupt_targets))
