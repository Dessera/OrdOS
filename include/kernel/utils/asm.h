#pragma once

#include "kernel/types.h"

static inline void
outb(u16 port, u8 data)
{
  __asm__ volatile("outb %b0, %w1" : : "a"(data), "Nd"(port));
}

static inline void
outsw(u16 port, const void* addr, u32 size)
{
  __asm__ volatile("cld;"
                   "rep outsw"
                   : "+S"(addr), "+c"(size)
                   : "d"(port));
}

static inline u8
inb(u16 port)
{
  u8 data = 0;
  __asm__ volatile("inb %w1, %b0" : "=a"(data) : "Nd"(port));
  return data;
}

static inline void
insw(u16 port, void* addr, u32 size)
{
  __asm__ volatile("cld;"
                   "rep insw"
                   : "+D"(addr), "+c"(size)
                   : "d"(port)
                   : "memory");
}

static inline void
sti(void)
{
  __asm__ volatile("sti");
}

static inline void
cli(void)
{
  __asm__ volatile("cli");
}

static inline void
lidt(u64 idt_ptr)
{
  __asm__ volatile("lidt %0" : : "m"(idt_ptr));
}