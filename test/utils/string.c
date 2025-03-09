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
  KASSERT_EXPR(kstrlen(str) == 13);

  // test kstrcmp
  KASSERT_EXPR(kstrcmp(str, str) == 0);
  KASSERT_EXPR(kstrcmp(str, str2) == 1);
  KASSERT_EXPR(kstrcmp(str, str3) == -1);

  // test kstrcpy
  kstrcpy(buf, str);
  KASSERT_EXPR(kstrcmp(buf, str) == 0);

  // test kstrcat
  kstrcat(buf, str2);
  kstrcat(buf, str3);
  KASSERT_EXPR(kstrcmp(buf, str4) == 0);

  // test kstrchr
  KASSERT_EXPR(kstrchr(str, 'o') == str + 4);

  // test kstrrchr
  KASSERT_EXPR(kstrrchr(str, 'o') == str + 8);

  // test kstrchrs
  KASSERT_EXPR(kstrchrs(str, 'l') == 3);
}