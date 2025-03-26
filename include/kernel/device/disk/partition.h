#pragma once

#include "kernel/device/disk/disk.h"
#include "kernel/utils/list_head.h"

struct disk_partition
{
  char name[DEVICE_DISK_PARTITION_NAME_SIZE];
  size_t sec_start;
  size_t sec_cnt;

  struct disk* disk;
  struct list_head node;

  // TODO: file system
};

void
init_partition(void);
