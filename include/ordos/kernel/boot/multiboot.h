/**
 * @file multiboot.h
 * @author Dessera (dessera@qq.com)
 * @brief Multiboot2 definitions.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#define MULTIBOOT_HEADER_MAGIC 0xE85250D6

#define MULTIBOOT_LOADER_MAGIC 0x36d76289

#define MULTIBOOT_HEADER_ARCH_1386 0

#define MULTIBOOT_TAG_ALIGN 8
#define MULTIBOOT_TAG_TYPE_END 0
#define MULTIBOOT_TAG_TYPE_CMDLINE 1
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME 2
#define MULTIBOOT_TAG_TYPE_MODULE 3
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO 4
#define MULTIBOOT_TAG_TYPE_BOOTDEV 5
#define MULTIBOOT_TAG_TYPE_MMAP 6
#define MULTIBOOT_TAG_TYPE_VBE 7
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER 8
#define MULTIBOOT_TAG_TYPE_ELF_SECTIONS 9
#define MULTIBOOT_TAG_TYPE_APM 10
#define MULTIBOOT_TAG_TYPE_EFI32 11
#define MULTIBOOT_TAG_TYPE_EFI64 12
#define MULTIBOOT_TAG_TYPE_SMBIOS 13
#define MULTIBOOT_TAG_TYPE_ACPI_OLD 14
#define MULTIBOOT_TAG_TYPE_ACPI_NEW 15
#define MULTIBOOT_TAG_TYPE_NETWORK 16
#define MULTIBOOT_TAG_TYPE_EFI_MMAP 17
#define MULTIBOOT_TAG_TYPE_EFI_BS 18
#define MULTIBOOT_TAG_TYPE_EFI32_IH 19
#define MULTIBOOT_TAG_TYPE_EFI64_IH 20
#define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR 21

#define MULTIBOOT_HEADER_TAG_TYPE_END 0
#define MULTIBOOT_HEADER_TAG_TYPE_INFORMATION_REQUEST 1
#define MULTIBOOT_HEADER_TAG_TYPE_ADDRESS 2
#define MULTIBOOT_HEADER_TAG_TYPE_ENTRY_ADDRESS 3
#define MULTIBOOT_HEADER_TAG_TYPE_CONSOLE_FLAGS 4
#define MULTIBOOT_HEADER_TAG_TYPE_FRAMEBUFFER 5
#define MULTIBOOT_HEADER_TAG_TYPE_MODULE_ALIGN 6
#define MULTIBOOT_HEADER_TAG_TYPE_EFI_BS 7
#define MULTIBOOT_HEADER_TAG_TYPE_ENTRY_ADDRESS_EFI32 8
#define MULTIBOOT_HEADER_TAG_TYPE_ENTRY_ADDRESS_EFI64 9
#define MULTIBOOT_HEADER_TAG_TYPE_RELOCATABLE 10

#define MULTIBOOT_HEADER_TAG_FLAG_OPTIONAL 1

#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5

#define MULTIBOOT_HEADER(name, isa)                                            \
  name:                                                                        \
  .align 8;                                                                    \
  .long MULTIBOOT_HEADER_MAGIC;                                                \
  .long isa;                                                                   \
  .long name##_end - name;                                                     \
  .long - (MULTIBOOT_HEADER_MAGIC + isa + name##_end - name);

#define MULTIBOOT_HEADER_END(name) name##_end:

#define MULTIBOOT_HEADER_TAG_EFI32_ENTRY(name, addr)                           \
  name:                                                                        \
  .align 8;                                                                    \
  .short MULTIBOOT_HEADER_TAG_TYPE_ENTRY_ADDRESS_EFI32;                        \
  .short MULTIBOOT_HEADER_TAG_FLAG_OPTIONAL;                                   \
  .long name##_end - name;                                                     \
  .long addr;                                                                  \
  name##_end:

#define MULTIBOOT_HEADER_TAG_BIOS_ENTRY(name, addr)                            \
  name:                                                                        \
  .align 8;                                                                    \
  .short MULTIBOOT_HEADER_TAG_TYPE_ENTRY_ADDRESS;                              \
  .short 0;                                                                    \
  .long name##_end - name;                                                     \
  .long addr;                                                                  \
  name##_end:

#define MULTIBOOT_HEADER_TAG_END(name)                                         \
  name:                                                                        \
  .align 8;                                                                    \
  .short MULTIBOOT_HEADER_TAG_TYPE_END;                                        \
  .short 0;                                                                    \
  .long name##_end - name;                                                     \
  name##_end:

#ifndef __ASSEMBLER__

#include "ordos/lib/types.h"

struct multiboot_info_header
{
  u32 total_size;
  u32 reserved;
};

struct multiboot_mmap_entry
{
  u64 addr;
  u64 len;
  u64 type;
};

struct multiboot_tag
{
  u32 type;
  u32 size;
};

struct multiboot_basic_meminfo
{
  struct multiboot_tag tag;
  u32 mem_lower;
  u32 mem_uppder;
};

struct multiboot_mmap
{
  struct multiboot_tag tag;
  u32 entry_size;
  u32 entry_version;
  struct multiboot_mmap_entry entries[0];
};

#endif
