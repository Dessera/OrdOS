#pragma once

#include "kernel/types.h"

static FORCE_INLINE void
outb(u16 port, u8 data)
{
  __asm__ volatile("outb %b0, %w1" : : "a"(data), "Nd"(port));
}

static FORCE_INLINE void
outsw(u16 port, const void* addr, u32 size)
{
  __asm__ volatile("cld;"
                   "rep outsw"
                   : "+S"(addr), "+c"(size)
                   : "d"(port));
}

static FORCE_INLINE u8
inb(u16 port)
{
  u8 data = 0;
  __asm__ volatile("inb %w1, %b0" : "=a"(data) : "Nd"(port));
  return data;
}

static FORCE_INLINE void
insw(u16 port, void* addr, u32 size)
{
  __asm__ volatile("cld;"
                   "rep insw"
                   : "+D"(addr), "+c"(size)
                   : "d"(port)
                   : "memory");
}

static FORCE_INLINE void
sti(void)
{
  __asm__ volatile("sti");
}

static FORCE_INLINE void
cli(void)
{
  __asm__ volatile("cli");
}

static FORCE_INLINE void
lidt(u64 idt_ptr)
{
  __asm__ volatile("lidt %0" : : "m"(idt_ptr));
}

static FORCE_INLINE u32
eflags(void)
{
  u32 eflags = 0;
  __asm__ volatile("pushfl;"
                   "popl %0"
                   : "=g"(eflags));
  return eflags;
}

static FORCE_INLINE void
hlt(void)
{
  __asm__ volatile("hlt");
}

static FORCE_INLINE void
invlpg(void* addr)
{
  __asm__ volatile("invlpg %0" : : "m"(addr) : "memory");
}
