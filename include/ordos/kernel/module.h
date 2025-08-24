#pragma once

#include "ordos/kernel/config.h"
#include "ordos/lib/common.h"

#define module_init(mname, mentry)                                             \
  __section(".data.module") struct module __##mname = {                        \
    .name = #mname, .entry = mentry, .magic = ORDOS_MODULE_MAGIC               \
  };

typedef void (*module_entry_t)(void);

struct module
{
  const char* name;
  module_entry_t entry;
  u32 magic;
};
