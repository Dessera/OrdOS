#include "kernel/memory/sslab/sslab.h"
#include "kernel/assert.h"
#include "lib/types.h"

struct test_object
{
  int a;
  int b;
  int c;
  int d;
};

DECLARE_WITH_PROTOTYPE(void, ktester, void)
{
  struct test_object* ptrs[100] = { 0 };

  for (int i = 0; i < 100; i++) {
    ptrs[i] = sslab_global_alloc(sizeof(struct test_object));
    *ptrs[i] =
      (struct test_object){ .a = i, .b = i * 2, .c = i * 3, .d = i * 4 };
  }

  for (int i = 0; i < 100; i++) {
    KASSERT(
      ptrs[i]->a == i, "in %p: a is not %d, but %d", ptrs[i], i, ptrs[i]->a);

    KASSERT(ptrs[i]->b == i * 2,
            "in %p: b is not %d, but %d",
            ptrs[i],
            i * 2,
            ptrs[i]->b);

    KASSERT(ptrs[i]->c == i * 3,
            "in %p: c is not %d, but %d",
            ptrs[i],
            i * 3,
            ptrs[i]->c);

    KASSERT(ptrs[i]->d == i * 4,
            "in %p: d is not %d, but %d",
            ptrs[i],
            i * 4,
            ptrs[i]->d);
  }

  for (int i = 0; i < 100; i++) {
    sslab_global_free(ptrs[i]);
  }
}