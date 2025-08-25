#pragma once

#include "ordos/kernel/config.h"
#include "ordos/kernel/module/version.h"

enum dep_require
{
  DEP_EQ,
  DEP_GREAT,
  DEP_GREATEQ,
  DEP_LESS,
  DEP_LESSEQ,
};

struct dependency
{
  char name[ORDOS_MODULE_NAME_LENGTH];
  struct version version;
  enum dep_require require;
};

void
dependency_init(struct dependency* dep,
                const char* name,
                struct version* version,
                enum dep_require require);

int
dependency_parse(struct dependency* dep, const char* dstr);
