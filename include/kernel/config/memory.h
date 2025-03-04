#pragma once

// Size of page table
#define PAGE_SIZE 0x1000

// Address of the page table
#define PAGE_TABLE_ADDR 0x100000

// Kernel pde offset
#define PDE_KERNEL_OFFSET 0xc00

// Factor number used to calculate the size of the kernel memory pool (divisor)
#define MEMPOOL_KERNEL_FACTOR 2

// Address of the memory pool bitmap
#define MEMPOOL_BITMAP_ADDR 0xc009a000