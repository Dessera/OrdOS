#include "kernel/utils/string.h"
#include "kernel/assert.h"

void
ktester(void);

void
ktester(void)
{
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
}