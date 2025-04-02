#pragma once

#include <stdio.h>
#include <sys/types.h>

struct image_partition
{
  size_t sec_start;
  size_t sec_cnt;

  struct image_partition* next;
  struct image_partition* prev;

  FILE* fp;
};

void
partition_list_add(struct image_partition* head, struct image_partition* p);

struct image_partition*
list_partitions(FILE* fp);

void
destroy_partition_list(struct image_partition* p);

bool
image_read_sec(FILE* fp, void* buf, size_t sec, size_t cnt);

bool
image_write_sec(FILE* fp, void* buf, size_t sec, size_t cnt);

bool
partition_create_fs(struct image_partition* p);

bool
partition_check_fs(struct image_partition* p);