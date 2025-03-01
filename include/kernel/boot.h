#pragma once

// Address of the boot sector
#define BOOTSEC_ADDR 0x7c00

// Address of the LDR sector
#define LDRSEC_ADDR 0x0900

// Address of the kernel sector
#define KERNELSEC_ADDR 0xc0008000

// Offset of the LDR entry in the LDR sector
#define LDRENTRY_OFFSET 0x0300

// Real address of the LDR entry
#define LDRENTRY_ADDR (LDRSEC_ADDR + LDRENTRY_OFFSET)

// LDR sector start
#define LDRSTART 0x01

// LDR sector size
#define LDRSIZE 0x04

// Kernel sector start
#define KERNELSTART (LDRSTART + LDRSIZE)

// Kernel sector size
#define KERNELSIZE 0x01
