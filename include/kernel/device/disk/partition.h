#pragma once

#include "kernel/device/disk/disk.h"
#include "kernel/utils/list_head.h"
#include "lib/types.h"

#define DISK_PARTITION_NAME_SIZE 8

struct disk_partition
{
  char name[DISK_PARTITION_NAME_SIZE];
  size_t sec_start;
  size_t sec_cnt;

  struct disk* disk;
  struct list_head node;

  // TODO: file system
};

void
init_partition(void);

void
disk_partition_scan(struct disk* disk);