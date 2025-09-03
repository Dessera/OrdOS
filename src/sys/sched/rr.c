#include "ordos/core/task.h"
#include "ordos/core/task/kthread.h"
#include "ordos/core/task/sched.h"
#include "ordos/core/task/tss.h"
#include "ordos/lib/asm.h"
#include "ordos/lib/assert.h"
#include "ordos/lib/error.h"
#include "ordos/lib/logging.h"
#include "ordos/lib/util/list_head.h"
#include "ordos/module.h"

static void
__force_sched_rr(struct list_head* ready_list)
{
  kassert(intr_get_status() == false,
          "sched: Interrupt is enabled when scheduling");

  struct task* curr = task_get_current();
  if (curr->status == TSK_RUNNING) {
    list_add_tail(&curr->node, ready_list);
    curr->vticks = curr->prio;
    curr->status = TSK_READY;
  }

  if (list_empty(ready_list)) {
    task_unpark(__kidle);
  }

  struct task* next = list_entry(list_pop(ready_list), struct task, node);
  task_set_current(next);

  lcr3(next->pd);
  tss_update_esp(next);

  _asm_task_switch_to(curr, next);
}

static void
__sched_rr(struct list_head* ready_list)
{
  struct task* curr = task_get_current();

  ++curr->ticks;

  if (curr->vticks != 0) {
    --curr->vticks;
  } else {
    __force_sched_rr(ready_list);
  }
}

int
sched_rr_entry(struct module* mod)
{
  if (sched_register("rr", __sched_rr, __force_sched_rr) != E_SUCCESS) {
    mwarn(mod, "Failed to insert scheduler");
    return E_LOAD;
  }

  return E_SUCCESS;
}

module_init_noexit(sched_rr, MOD_SCHED | MOD_AUTOLOAD, sched_rr_entry);
