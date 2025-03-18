#pragma once

#define BOOT_MBR_START 0x7c00

#define BOOT_LDR_START 0x900
#define BOOT_LDR_SIZE 0x04

// interval to control the compile size of the bootloader
#define BOOT_LDR_SIZE_INTERVAL 0x01

#define BOOT_KERNEL_START 0x1000000
#define BOOT_KERNEL_SIZE 0x25

// interval to control the compile size of the kernel
#define BOOT_KERNEL_SIZE_INTERVAL 0x08