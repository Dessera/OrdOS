#pragma once

#include "kernel/config/boot.h"
#include "kernel/config/memory.h"

#define BOOT_SEC_SIZE 0x200

#define BOOT_LDR_SEC_START 0x01

#define BOOT_KERNEL_SEC_START (BOOT_LDR_SIZE + BOOT_LDR_SEC_START)

#define BOOT_KERNEL_ENTRY_START (MEM_KERNEL_VSTART + BOOT_KERNEL_START)