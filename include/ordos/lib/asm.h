/**
 * @file asm.h
 * @author Dessera (dessera@qq.com)
 * @brief Some assembly commands.
 * @version 0.1.0
 * @date 2025-08-13
 *
 * @copyright Copyright (c) 2025 Dessera
 *
 */

#pragma once

#include "ordos/lib/common.h"

static ORDOS_FORCEINLINE void
outb(u16 port, u8 data) // NOLINT
{
  asm_exec("outb %b0, %w1" : : "a"(data), "Nd"(port));
}

static ORDOS_FORCEINLINE void
outsw(u16 port, const void* addr, u32 size)
{
  asm_exec("cld;"
           "rep outsw" : "+S"(addr),
           "+c"(size) : "d"(port));
}

static ORDOS_FORCEINLINE u8
inb(u16 port)
{
  u8 data = 0;
  asm_exec("inb %w1, %b0" : "=a"(data) : "Nd"(port));
  return data;
}

static ORDOS_FORCEINLINE void
insw(u16 port, void* addr, u32 size)
{
  asm_exec("cld;"
           "rep insw" : "+D"(addr),
           "+c"(size) : "d"(port) : "memory");
}

static ORDOS_FORCEINLINE void
sti(void)
{
  asm_exec("sti");
}

static ORDOS_FORCEINLINE void
cli(void)
{
  asm_exec("cli");
}

static ORDOS_FORCEINLINE void
lidt(u64 idt_ptr)
{
  asm_exec("lidt %0" : : "m"(idt_ptr));
}

static ORDOS_FORCEINLINE u32
eflags(void)
{
  u32 eflags = 0;
  asm_exec("pushfl;"
           "popl %0" : "=g"(eflags));
  return eflags;
}

static ORDOS_FORCEINLINE void
hlt(void)
{
  asm_exec("hlt");
}

static ORDOS_FORCEINLINE void
invlpg(void* addr)
{
  asm_exec("invlpg %0" : : "m"(addr) : "memory");
}

static ORDOS_FORCEINLINE void
lcr3(void* addr)
{
  asm_exec("movl %0, %%cr3" : : "r"(addr));
}

static ORDOS_FORCEINLINE u32
rcr0(void)
{
  u32 cr0 = 0;
  asm_exec("movl %%cr0, %0" : "=r"(cr0));
  return cr0;
}

static ORDOS_FORCEINLINE void
lcr0(u32 cr0)
{
  asm_exec("movl %0, %%cr0" : : "r"(cr0));
}

static ORDOS_FORCEINLINE u32
rcr2(void)
{
  u32 cr2 = 0;
  asm_exec("movl %%cr2, %0" : "=r"(cr2));
  return cr2;
}
