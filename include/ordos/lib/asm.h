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

__inline static void
outb(u16 port, u8 data)
{
  asm_exec("outb %b0, %w1" : : "a"(data), "Nd"(port));
}

__inline static void
outsw(u16 port, const void* addr, u32 size)
{
  asm_exec("cld;"
           "rep outsw" : "+S"(addr),
           "+c"(size) : "d"(port));
}

__inline static u8
inb(u16 port)
{
  u8 data = 0;
  asm_exec("inb %w1, %b0" : "=a"(data) : "Nd"(port));
  return data;
}

__inline static void
insw(u16 port, void* addr, u32 size)
{
  asm_exec("cld;"
           "rep insw" : "+D"(addr),
           "+c"(size) : "d"(port) : "memory");
}

__inline static void
sti(void)
{
  asm_exec("sti");
}

__inline static void
cli(void)
{
  asm_exec("cli");
}

__inline static void
lidt(u64 idt_ptr)
{
  asm_exec("lidt %0" : : "m"(idt_ptr));
}

__inline static u32
eflags(void)
{
  u32 eflags = 0;
  asm_exec("pushfl;"
           "popl %0" : "=g"(eflags));
  return eflags;
}

__inline static void
hlt(void)
{
  asm_exec("hlt");
}

__inline static void
invlpg(void* addr)
{
  asm_exec("invlpg %0" : : "m"(addr) : "memory");
}

__inline static void
lcr3(void* addr)
{
  asm_exec("movl %0, %%cr3" : : "r"(addr));
}

__inline static u32
rcr0(void)
{
  u32 cr0 = 0;
  asm_exec("movl %%cr0, %0" : "=r"(cr0));
  return cr0;
}

__inline static void
lcr0(u32 cr0)
{
  asm_exec("movl %0, %%cr0" : : "r"(cr0));
}

__inline static u32
rcr2(void)
{
  u32 cr2 = 0;
  asm_exec("movl %%cr2, %0" : "=r"(cr2));
  return cr2;
}
