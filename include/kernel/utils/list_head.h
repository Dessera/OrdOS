#pragma once

#include "kernel/stddef.h"

struct list_head
{
  struct list_head *next, *prev;
};

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

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_for_each(pos, head)                                               \
  for ((pos) = (head)->next; (pos) != (head); (pos) = (pos)->next)
