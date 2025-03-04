#pragma once

// Address of the boot sector
#define BOOTSEC_ADDR 0x7c00

// Address of the LDR sector
#define LDRSEC_ADDR 0x0900

// Address of the kernel sector
#define KERNELSEC_ADDR 0x8000

// Offset of the LDR entry in the LDR sector
#define LDRENTRY_OFFSET 0x0300

// LDR sector size
#define LDRSIZE 0x04

// Kernel sector size
#define KERNELSIZE 0x10