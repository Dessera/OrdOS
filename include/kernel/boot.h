#pragma once

#include "config.h"

// Real address of the LDR entry
#define LDRENTRY_ADDR (LDRSEC_ADDR + LDRENTRY_OFFSET)

// Real address of the kernel entry
#define KERNELENTRY_ADDR (KERNELBASE + KERNELSEC_ADDR)

// LDR sector start
#define LDRSTART (MBRSIZE)

// Kernel sector start
#define KERNELSTART (LDRSTART + LDRSIZE)
