#pragma once

#include "kernel/defs.h"
#include "lib/types.h"

/**
 * @brief IDT descriptor type
 */
enum idt_desc_type
{
  IDT_DESC_TYPE_INT = 0xe,
};

/**
 * @brief IDT descriptor
 */
struct idt_desc
{
  u16 offs_lb;
  u16 sel;
  u8 reserved;
  u8 attrs;
  u16 offs_hb;
} __attribute__((packed));

/**
 * @brief Initializes the IDT
 */
void
init_idt(void);

/**
 * @brief Initializes an IDT descriptor
 *
 * @param desc IDT descriptor
 * @param entry Interrupt handler
 * @param sel Segment selector
 * @param type IDT descriptor type
 * @param dpl Descriptor privilege level
 */
void
idt_desc_init(struct idt_desc* desc,
              void* entry,
              u16 sel,
              enum idt_desc_type type,
              enum dpl dpl);

/**
 * @brief Gets the register pointer to the IDT
 *
 * @param idt_addr Address of the IDT
 * @return u64 Register pointer to the IDT
 */
static FORCE_INLINE u64
idt_get_ptr(struct idt_desc* idt_addr, size_t size)
{
  return (((u64)(uintptr_t)idt_addr << 16) |
          (size * sizeof(struct idt_desc) - 1));
}
