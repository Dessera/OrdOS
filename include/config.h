#pragma once

// Address of the boot sector
#define BOOTSEC_ADDR 0x7c00

// Address of the LDR sector
#define LDRSEC_ADDR 0x0900

// Address of the kernel sector
#define KERNELSEC_ADDR 0x8000

// Address of the kernel base
#define KERNELBASE 0xc0000000

// Offset of the LDR entry in the LDR sector
#define LDRENTRY_OFFSET 0x0300

// MBR sector size (must be 1)
#define MBRSIZE 0x01

// LDR sector size
#define LDRSIZE 0x04

// Kernel sector size
#define KERNELSIZE 0x01

// Size of page table
#define PAGE_SIZE 0x1000

// Address of the page table
#define PAGE_TABLE_ADDR 0x100000