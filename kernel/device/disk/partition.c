#include "kernel/device/disk/partition.h"
#include "kernel/device/disk/disk.h"
#include "kernel/log.h"
#include "kernel/memory/memory.h"
#include "kernel/utils/print.h"
#include "lib/list_head.h"
#include "lib/string.h" // IWYU pragma: keep
#include "lib/types.h"

struct list_head __partition_list;

static void
__disk_partition_scan_impl(struct disk* disk,
                           size_t sec_start,
                           size_t sec_base,
                           size_t* pt_cnt)
{
  struct boot_sector* bs = kmalloc(sizeof(struct boot_sector));
  if (bs == nullptr) {
    KWARNING("no memory for boot sector when scanning partitions");
    return;
  }

  disk_read(disk, sec_start, 1, bs);

  AUTO partition_table = bs->partition_table;

  for (size_t part_idx = 0; part_idx < 4; part_idx++) {
    if (partition_table[part_idx].fs_type == 0x05) {
      if (sec_base == 0) {
        __disk_partition_scan_impl(disk,
                                   partition_table[part_idx].start_lba,
                                   partition_table[part_idx].start_lba,
                                   pt_cnt);
      } else {
        __disk_partition_scan_impl(disk,
                                   partition_table[part_idx].start_lba +
                                     sec_base,
                                   sec_base,
                                   pt_cnt);
      }
    } else if (partition_table[part_idx].fs_type != 0x00) {
      struct disk_partition* partition = kmalloc(sizeof(struct disk_partition));
      partition->disk = disk;
      partition->sec_start = partition_table[part_idx].start_lba + sec_start;
      partition->sec_cnt = partition_table[part_idx].sec_cnt;
      partition->logical = sec_start != 0;
      ksprint(partition->name, "%s%u", disk->name, *pt_cnt);
      list_add_tail(&partition->node, &__partition_list);

      (*pt_cnt)++;
    }
  }

  kfree(bs);
}

static void
__disk_partition_scan(struct disk* disk)
{
  size_t pt_cnt = 0;
  __disk_partition_scan_impl(disk, 0, 0, &pt_cnt);
}

void
init_partition(void)
{
  list_init(&__partition_list);
  for (size_t i = 0; i < disk_get_cnt(); i++) {
    if (i != DEVICE_DISK_KERNEL_INDEX) {
      __disk_partition_scan(disk_get(i));
    }
  }
}

struct disk_partition*
partition_get_by_name(const char* name)
{
  struct list_head* entry;
  LIST_FOR_EACH(entry, &__partition_list)
  {
    AUTO partition = LIST_ENTRY(entry, struct disk_partition, node);
    if (strcmp(partition->name, name) == 0) {
      return partition;
    }
  }
  return nullptr;
}

struct disk_partition*
partition_get_by_index(size_t index)
{
  size_t cnt = 0;
  struct list_head* entry;
  LIST_FOR_EACH(entry, &__partition_list)
  {
    AUTO partition = LIST_ENTRY(entry, struct disk_partition, node);
    if (cnt == index) {
      return partition;
    }
    cnt++;
  }

  return nullptr;
}

size_t
partition_get_index(struct disk_partition* partition)
{
  size_t cnt = 0;
  struct list_head* entry;
  LIST_FOR_EACH(entry, &__partition_list)
  {
    AUTO p = LIST_ENTRY(entry, struct disk_partition, node);
    if (p == partition) {
      return cnt;
    }
    cnt++;
  }

  return NPOS;
}

size_t
partition_get_cnt(void)
{
  size_t cnt = 0;
  struct list_head* entry;
  LIST_FOR_EACH(entry, &__partition_list)
  {
    cnt++;
  }
  return cnt;
}
