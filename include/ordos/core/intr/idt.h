/**
 * @file idt.h
 * @author Dessera (dessera@qq.com)
 * @brief IDT descriptors.
 * @version 0.1.0
 * @date 2025-08-24
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/lib/common.h" // IWYU pragma: keep
#include "ordos/lib/types.h"

#define id_desc_init(idt, entry, selector, flags)                              \
  do {                                                                         \
    idt.offs_lb = ((uintptr_t)(entry)) & WORDMASK;                             \
    idt.sel = selector;                                                        \
    idt.reserved = 0;                                                          \
    idt.attrs = flags;                                                         \
    idt.offs_hb = ((uintptr_t)(entry)) >> WORD;                                \
  } while (0)

/**
 * @brief IDT descriptor
 */
struct idt
{
  u16 offs_lb;
  u16 sel;
  u8 reserved;
  u8 attrs;
  u16 offs_hb;
};

/**
 * @brief IDT flags.
 *
 */
enum idt_flag
{
  IDT_TASKGATE = 0x04, /**< IDT is task gate. */
  IDT_INTRGATE = 0x0E, /**< IDT is interrupt gate. */
  IDT_TRAPGATE = 0x0F, /**< IDT is trap gate. */
  IDT_KERNEL = 0x0,    /**< IDT is for kernel space. */
  IDT_USER = 0x60,     /**< IDT is for user space. */
  IDT_PRESENT = 0x80,  /**< IDT is available. */
};

/**
 * @brief Initializes the IDT
 */
void
init_idt(void);

/**
 * @brief Gets the register pointer to the IDT.
 *
 * @param idt_addr Address of the IDT.
 * @return u64 Register pointer to the IDT.
 */
__inline static u64
idt_get_ptr(struct idt* idt_addr, size_t size)
{
  return (size * sizeof(struct idt) - 1) | ((u64)(uintptr_t)idt_addr << WORD);
}
