#include "kernel/device/disk/partition.h"
#include "kernel/device/disk/disk.h"
#include "kernel/log.h"
#include "kernel/memory/memory.h"
#include "kernel/utils/print.h"
#include "lib/list_head.h"
#include "lib/types.h"

struct list_head __partition_list;

struct partition_table_entry
{
  u8 bootable;
  u8 start_head;
  u8 start_sec;
  u8 start_chs;
  u8 fs_type;
  u8 end_head;
  u8 end_sec;
  u8 end_chs;
  u32 start_lba;
  u32 sec_cnt;
} __attribute__((packed));

struct boot_sector
{
  u8 other[446];
  struct partition_table_entry partition_table[4];
  u16 signature;
} __attribute__((packed));

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

#ifdef DEBUG
  struct list_head* entry;
  LIST_FOR_EACH(entry, &__partition_list)
  {
    AUTO partition = LIST_ENTRY(entry, struct disk_partition, node);
    KDEBUG("partition %s: start=%u, cnt=%u",
           partition->name,
           partition->sec_start,
           partition->sec_cnt);
  }
#endif
}
