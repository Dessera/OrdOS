#include "ordos/kernel/boot/multiboot.h"
#include "ordos/kernel/config.h"
#include "ordos/kernel/init.h"
#include "ordos/kernel/mem/map.h"
#include "ordos/kernel/utils.h"
#include "ordos/lib/common.h"
#include "ordos/lib/types.h"

ORDOS_SECTION(".data.init")
static const char* __err_mismatched_magic = "Invalid multiboot2 magic.";

ORDOS_SECTION(".text.init")
static int
__init_mmap(struct init_info* init, struct multiboot_mmap* mmap)
{
  for (struct multiboot_mmap_entry* entry = mmap->entries;
       (void*)entry < pcast_offs(mmap, mmap->tag.size);
       entry = pcast_offs(entry, mmap->entry_size)) {
    if (init->mmap_cnt >= ORDOS_INIT_MMAP_LENGTH) {
      break;
    }

    init->mmap[init->mmap_cnt].type = entry->type;
    init->mmap[init->mmap_cnt].addr = entry->addr;
    init->mmap[init->mmap_cnt].len = entry->len;
    ++init->mmap_cnt;
  }

  return 0;
}

ORDOS_SECTION(".text.init")
static int
__init_from_multiboot(struct init_info* init, void* tags)
{
  struct multiboot_mmap* mmap = NULL;

  for (struct multiboot_tag* tag = tags; tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = pcast_offs(tag, align_up(tag->size, 8))) {
    // find mmap
    if (tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
      mmap = (struct multiboot_mmap*)tag;
    }
  }

  if (mmap == NULL) {
    return 1;
  }

  if (__init_mmap(init, mmap) != 0) {
    return 1;
  }

  return 0;
}

ORDOS_SECTION(".text.init")
ORDOS_ASMLINKAGE int
_kprelude(u32 magic, void* info)
{
  struct init_info* init = vaccess(&__init);

  if (unlikely(magic != MULTIBOOT_LOADER_MAGIC)) {
    init->error = __err_mismatched_magic;
    return 1;
  }

  return __init_from_multiboot(
    init, pcast_offs(info, sizeof(struct multiboot_info_header)));
}
