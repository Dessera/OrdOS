#include "ordos/core/task/sched.h"
#include "ordos/config.h"
#include "ordos/lib/error.h"
#include "ordos/lib/string.h" // IWYU pragma: keep
#include "ordos/lib/types.h"  // IWYU pragma: keep

static struct sched __sched_list[ORDOS_TASK_SCHED_CNT];

static size_t __sched_cnt = 0;

int
sched_register(const char* name, task_sched_t entry, task_sched_t fentry)
{
  if (__sched_cnt >= ORDOS_TASK_SCHED_CNT) {
    return E_OVERFLOW;
  }

  strncpy(__sched_list[__sched_cnt].name, name, ORDOS_TASK_SCHED_NAME_LENGTH);
  __sched_list[__sched_cnt].entry = entry;
  __sched_list[__sched_cnt].fentry = fentry;

  ++__sched_cnt;

  return E_SUCCESS;
}

struct sched*
sched_find(const char* name)
{
  for (size_t i = 0; i < __sched_cnt; ++i) {
    if (strcmp(name, __sched_list[i].name) == 0) {
      return &__sched_list[i];
    }
  }

  return NULL;
}
