#pragma once
#include "lib/common.h"

// if no KVERSION and KNAME are defined, use default
#ifndef KVERSION
#define KVERSION Unknown
#endif

#ifndef KNAME
#define KNAME Kernel
#endif

// full version string
#define KFULLNAME KNAME KVERSION
#define KFULLNAME_STR STRINGIFY(KFULLNAME)
