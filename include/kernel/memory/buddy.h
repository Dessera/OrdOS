#pragma once

#include "lib/types.h"

struct page
{
  size_t ref_cnt;
  u8 zone : 2, reserved : 1, buddy : 1;
  u8 order;
};

void
init_buddy(void);