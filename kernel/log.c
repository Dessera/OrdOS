#include "kernel/log.h"
#include "kernel/device/vga.h"
#include "kernel/task/sync.h"

const char* debug_level_str[7] = { "NONE",  "ERROR", "WARNING", "INFO",
                                   "DEBUG", "TRACE", "PANIC" };
struct lock_t klog_lock;

void
init_log(void)
{
  init_vga();

  lock_init(&klog_lock);
}