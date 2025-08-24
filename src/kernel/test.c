#include "ordos/kernel/logging.h"
#include "ordos/kernel/module.h"

void
test_entry(void)
{
  kinfo("this is a test module!");
}

void
test_exit(void)
{
  kinfo("test exit");
}

module_init(test, test_entry, test_exit)
