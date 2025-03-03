#include "kernel/utils/string.h"
#include "kernel/assert.h"
#include "kernel/device/vga.h"
#include "kernel/utils/asm.h"
#include "kernel/utils/print.h"

void
kmain(void);

static void
kinit(void)
{
  init_vga();
}

void
kmain(void)
{

  kinit();

  // declare this in the stack
  // char buf[50] = { 0 };
  char buf[50];
  const char* str = "Hello, world!";
  const char* str2 = "Hello, xorld!";
  const char* str3 = "Hello, vorld!";
  const char* str4 = "Hello, world!Hello, xorld!Hello, vorld!";

  // test kstrlen
  KASSERT(kstrlen(str) == 13);

  // test kstrcmp
  KASSERT(kstrcmp(str, str) == 0);
  KASSERT(kstrcmp(str, str2) == 1);
  KASSERT(kstrcmp(str, str3) == -1);

  // test kstrcpy
  kstrcpy(buf, str);
  KASSERT(kstrcmp(buf, str) == 0);

  // test kstrcat
  kstrcat(buf, str2);
  kstrcat(buf, str3);
  KASSERT_MSG(kstrcmp(buf, str4) == 0, buf);

  // test kstrchr
  KASSERT(kstrchr(str, 'o') == str + 4);

  // test kstrrchr
  KASSERT(kstrrchr(str, 'o') == str + 8);

  // test kstrchrs
  KASSERT(kstrchrs(str, 'l') == 3);

  kputs("All tests passed!");

  hlt();
}