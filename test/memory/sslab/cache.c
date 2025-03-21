#include "kernel/memory/sslab/cache.h"
#include "kernel/assert.h"
#include "kernel/log.h"
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
  AUTO cache = sslab_cache_create(sizeof(struct test_object));

  KINFO("sslab cache: size %u", cache->obj_cnt);

  for (size_t i = 0; i < cache->obj_cnt; i++) {
    AUTO obj = sslab_cache_alloc(cache);
    if (obj == NULL) {
      KPANIC("sslab cache alloc failed");
    }
    struct test_object* t = (struct test_object*)obj;
    *t = (struct test_object){ i, i, i, i };

    sslab_cache_free(cache, obj);
  }
}