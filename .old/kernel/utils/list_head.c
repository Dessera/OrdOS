#include "kernel/utils/list_head.h"
#include "kernel/assert.h"
#include "lib/types.h"

void
list_init(struct list_head* head)
{
  KASSERT(head != NULL, "invalid head pointer when initializing list head");
  head->next = head;
  head->prev = head;
}

void
list_add(struct list_head* new, struct list_head* head)
{
  KASSERT(new != NULL, "invalid new pointer when adding to list");
  KASSERT(head != NULL, "invalid head pointer when adding to list");

  new->next = head->next;
  new->prev = head;
  head->next->prev = new;
  head->next = new;
}

void
list_add_tail(struct list_head* new, struct list_head* head)
{
  KASSERT(new != NULL, "invalid new pointer when adding to list");
  KASSERT(head != NULL, "invalid head pointer when adding to list");

  new->next = head;
  new->prev = head->prev;
  head->prev->next = new;
  head->prev = new;
}

void
list_del(struct list_head* entry)
{
  KASSERT(entry != NULL, "invalid entry pointer when deleting from list");

  entry->prev->next = entry->next;
  entry->next->prev = entry->prev;
}

bool
list_empty(struct list_head* head)
{
  KASSERT(head != NULL, "invalid head pointer when checking if list is empty");

  return head->next == head;
}

struct list_head*
list_pop(struct list_head* head)
{
  KASSERT(head != NULL, "invalid head pointer when popping from list");

  if (list_empty(head)) {
    return NULL;
  }

  struct list_head* entry = head->next;
  list_del(entry);
  return entry;
}

ssize_t
list_find(struct list_head* head, struct list_head* entry)
{
  KASSERT(head != NULL, "invalid head pointer when finding entry in list");
  KASSERT(entry != NULL, "invalid entry pointer when finding entry in list");

  ssize_t index = 0;
  struct list_head* current;
  LIST_FOR_EACH(current, head)
  {
    if (current == entry) {
      return index;
    }
    index++;
  }
  return -1;
}