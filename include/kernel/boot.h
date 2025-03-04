#pragma once

#include "kernel/config/general.h"
#include "kernel/config/memory.h"

// Address of the kernel base
#define KERNELBASE (PDE_KERNEL_OFFSET << 20)

// Real address of the LDR entry
#define LDRENTRY_ADDR (LDRSEC_ADDR + LDRENTRY_OFFSET)

// Real address of the kernel entry
#define KERNELENTRY_ADDR (KERNELBASE + KERNELSEC_ADDR)

// LDR sector start
#define LDRSTART 0x01

// Kernel sector start
#define KERNELSTART (LDRSTART + LDRSIZE)
