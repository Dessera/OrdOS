#include "ordos/kernel/module/deps.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/error.h"
#include "ordos/kernel/module/version.h"
#include "ordos/lib/string.h" // IWYU pragma: keep
#include "ordos/lib/types.h"  // IWYU pragma: keep

size_t
__parse_require(enum dep_require* require, const char* dstr, size_t index)
{
  size_t nindex = index;

  if (dstr[index] == '>') {
    if (dstr[index + 1] == '=') {
      *require = DEP_GREATEQ;
      ++nindex;
    } else {
      *require = DEP_GREAT;
    }
    ++nindex;
  } else if (dstr[index] == '<') {
    if (dstr[index + 1] == '=') {
      *require = DEP_LESSEQ;
      ++nindex;
    } else {
      *require = DEP_LESS;
    }
    ++nindex;
  } else {
    if (dstr[index + 1] == '=') {
      ++nindex;
    }
    *require = DEP_EQ;
    ++nindex;
  }

  return nindex;
}

void
dependency_init(struct dependency* dep,
                const char* name,
                struct version* version,
                enum dep_require require)
{
  strncpy(dep->name, name, ORDOS_MODULE_NAME_LENGTH);
  version_init(&dep->version, version->major, version->minor, version->fix);
  dep->require = require;
}

int
dependency_parse(struct dependency* dep, const char* dstr)
{
  char dbuf[ORDOS_MODULE_BUFSIZE] = { 0 };

  size_t index = 0;
  size_t len = strlen(dstr);

  strncpy(dbuf, dstr, len);

  while (dstr[index] != '>' && dstr[index] != '<' && dstr[index] != '=' &&
         index < len) {
    ++index;
  }

  if (index >= len - 1) {
    return E_PARSE;
  }

  size_t nindex = __parse_require(&dep->require, dbuf, index);
  dbuf[index] = '\0';

  strncpy(dep->name, dbuf, ORDOS_MODULE_NAME_LENGTH);

  int status = version_parse(&dep->version, &dbuf[nindex]);
  if (status != E_SUCCESS) {
    return status;
  }

  return 0;
}
