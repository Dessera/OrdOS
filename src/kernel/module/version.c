#include "ordos/kernel/module/version.h"
#include "ordos/kernel/error.h"
#include "ordos/lib/string.h" // IWYU pragma: keep
#include "ordos/lib/types.h"

static int
__parse_version_num(u8* res, const char* vstr, size_t len, size_t* index)
{
  while (vstr[*index] != '.' && *index < len) {
    if (!is_digit(vstr[*index])) {
      return E_PARSE;
    }

    int num = to_digit(vstr[*index]);
    if ((U8_MAX - num) / BASE_DEC <= *res) {
      return E_PARSE;
    }

    *res = *res * BASE_DEC + num;
    ++(*index);
  }

  return E_SUCCESS;
}

static int
__compare_version(u8 lhs, u8 rhs)
{
  if (lhs > rhs) {
    return 1;
  }
  if (lhs < rhs) {
    return -1;
  }

  return 0;
}

void
version_init(struct version* version, u8 major, u8 minor, u8 fix)
{
  version->major = major;
  version->minor = minor;
  version->fix = fix;
}

int
version_parse(struct version* version, const char* vstr)
{
  int status = E_SUCCESS;
  size_t len = strlen(vstr);
  size_t index = 0;

  u8 major = 0;
  u8 minor = 0;
  u8 fix = 0;

  status = __parse_version_num(&major, vstr, len, &index);
  if (status != E_SUCCESS) {
    return status;
  }

  if (index >= len) {
    goto parse_end;
  } else {
    ++index;
  }

  status = __parse_version_num(&minor, vstr, len, &index);
  if (status != E_SUCCESS) {
    return status;
  }

  if (index >= len) {
    goto parse_end;
  } else {
    ++index;
  }

  status = __parse_version_num(&fix, vstr, len, &index);
  if (status != E_SUCCESS) {
    return status;
  }

parse_end:
  version_init(version, major, minor, fix);

  return E_SUCCESS;
}

int
version_compare(struct version* lhs, struct version* rhs)
{
  int status = 0;

  status = __compare_version(lhs->major, lhs->major);
  if (status != 0) {
    return status;
  }

  status = __compare_version(lhs->minor, lhs->minor);
  if (status != 0) {
    return status;
  }

  return __compare_version(lhs->fix, rhs->fix);
}
