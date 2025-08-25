#pragma once

#include "ordos/lib/types.h"

struct version
{
  u8 major;
  u8 minor;
  u8 fix;
};

void
version_init(struct version* version, u8 major, u8 minor, u8 fix);

int
version_parse(struct version* version, const char* vstr);

int
version_compare(struct version* lhs, struct version* rhs);
