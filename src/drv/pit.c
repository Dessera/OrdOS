#include "ordos/drv/pit.h"
#include "ordos/core/intr.h"
#include "ordos/core/mem.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/module.h"
#include "ordos/lib/error.h"
#include "ordos/lib/logging.h"
#include "ordos/lib/sync.h"
#include "ordos/lib/types.h"
#include "ordos/lib/util/list_head.h"

enum pit_io_port : u16
{
  PIT_CHANNEL0 = 0x40,
  PIT_CHANNEL1 = 0x41,
  PIT_CHANNEL2 = 0x42,
  PIT_MODEREG = 0x43,
};

enum pit_flag : u8
{
  PIT_SEL_C0 = 0x00,
  PIT_SEL_C1 = 0x40,
  PIT_SEL_C2 = 0x80,

  PIT_LATCHCNT = 0x00,
  PIT_LBONLY = 0x10,
  PIT_HBONLY = 0x20,
  PIT_LBHB = 0x30,

  PIT_INTR = 0x00,
  PIT_ONESHOT = 0x02,
  PIT_RATE = 0x04,
  PIT_SQUARE = 0x06,
  PIT_SWSTROBE = 0x08,
  PIT_HWSTROBE = 0x0A,

  PIT_BIN = 0x00,
  PIT_BCD = 0x01,
};

struct tick_watcher
{
  size_t ticks;
  pit_handler_t handler;
  struct list_head node;
};

static struct list_head __watchers_list;

static u16 __freq = ORDOS_SYS_DEFAULT_FREQ;
static size_t __ticks = 0;

__inline static void
__pit_write(enum pit_io_port port, u8 data)
{
  outb(port, data);
}

static void
__pit_intr_handler(enum intr_type code)
{
  (void)code;

  if (__ticks == ORDOS_DEV_PIT_MAX_TICKS) {
    __ticks = 1;
  } else {
    ++__ticks;
  }

  struct list_head* entry = NULL;
  list_for_each(entry, &__watchers_list)
  {
    struct tick_watcher* watcher_entry =
      list_entry(entry, struct tick_watcher, node);

    if (__ticks % watcher_entry->ticks == 0) {
      watcher_entry->handler(__ticks);
    }
  }
}

u16
get_sys_freq(void)
{
  return __freq;
}

void
set_sys_freq(u16 freq)
{
  u16 cnt = PIT_INPUT_FREQ / freq;

  bool status = intr_lock();

  __pit_write(PIT_MODEREG, PIT_SEL_C0 | PIT_INTR | PIT_LBHB | PIT_RATE);
  __pit_write(PIT_CHANNEL0, cnt & BYTEMASK);
  __pit_write(PIT_CHANNEL0, (cnt >> BYTE) & BYTEMASK);

  __freq = freq;

  intr_unlock(status);
}

size_t
get_ticks(void)
{
  return __ticks;
}

int
pit_register(size_t ticks, pit_handler_t handler)
{
  struct tick_watcher* watcher = kmalloc(sizeof(struct tick_watcher));
  if (watcher == NULL) {
    return E_ALLOC;
  }

  if (ticks > ORDOS_DEV_PIT_MAX_TICKS) {
    kwarn("PIT: Invalid tick watcher on %u ticks", ticks);
    return E_OVERFLOW;
  }

  watcher->ticks = ticks;
  watcher->handler = handler;

  bool status = intr_lock();

  list_add(&watcher->node, &__watchers_list);

  intr_unlock(status);
  return E_SUCCESS;
}

int
pit_entry(struct module* mod)
{
  list_init(&__watchers_list);
  intr_register(IDRV_PIT, __pit_intr_handler);

  set_sys_freq(__freq);

  kinfo("%s: Init with sysfreq: %u", mod->name, __freq);
  return E_SUCCESS;
}

module_init_noexit(drv_pit, MOD_COREMOD, pit_entry);
