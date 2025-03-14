#pragma once

#include "lib/types.h"

#define SECTION(name) __attribute__((section(name)))

#define ASM(...) __asm__ __volatile__(__VA_ARGS__)

static FORCE_INLINE void
outb(u16 port, u8 data)
{
  ASM("outb %b0, %w1" : : "a"(data), "Nd"(port));
}

static FORCE_INLINE void
outsw(u16 port, const void* addr, u32 size)
{
  ASM("cld;"
      "rep outsw" : "+S"(addr),
      "+c"(size) : "d"(port));
}

static FORCE_INLINE u8
inb(u16 port)
{
  u8 data = 0;
  ASM("inb %w1, %b0" : "=a"(data) : "Nd"(port));
  return data;
}

static FORCE_INLINE void
insw(u16 port, void* addr, u32 size)
{
  ASM("cld;"
      "rep insw" : "+D"(addr),
      "+c"(size) : "d"(port) : "memory");
}

static FORCE_INLINE void
sti(void)
{
  ASM("sti");
}

static FORCE_INLINE void
cli(void)
{
  ASM("cli");
}

static FORCE_INLINE void
lidt(u64 idt_ptr)
{
  ASM("lidt %0" : : "m"(idt_ptr));
}

static FORCE_INLINE u32
eflags(void)
{
  u32 eflags = 0;
  ASM("pushfl;"
      "popl %0" : "=g"(eflags));
  return eflags;
}

static FORCE_INLINE void
hlt(void)
{
  ASM("hlt");
}

static FORCE_INLINE void
invlpg(void* addr)
{
  ASM("invlpg %0" : : "m"(addr) : "memory");
}
