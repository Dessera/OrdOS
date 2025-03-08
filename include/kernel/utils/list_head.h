#pragma once

#include "kernel/types.h"

struct list_head
{
  struct list_head *next, *prev;
};

#define LIST_ENTRY(ptr, type, member) CONTAINER_OF(ptr, type, member)

void
list_init(struct list_head* head);

void
list_add(struct list_head* new, struct list_head* head);

void
list_add_tail(struct list_head* new, struct list_head* head);

void
list_del(struct list_head* entry);

bool
list_empty(struct list_head* head);

struct list_head*
list_pop(struct list_head* head);
