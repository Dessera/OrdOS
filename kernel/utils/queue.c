

#include "kernel/utils/queue.h"
#include "kernel/task/sync.h"
#include "kernel/utils/list_head.h"

void
atomic_queue_init(struct atomic_queue* q)
{
  list_init(&q->head);
  mutex_lock_init(&q->lock);
}

void
atomic_queue_push(struct atomic_queue* q, struct list_head* item)
{
  mutex_lock(&q->lock);
  list_add_tail(item, &q->head);
  mutex_unlock(&q->lock);
}

struct list_head*
atomic_queue_pop(struct atomic_queue* q)
{
  struct list_head* item = NULL;
  mutex_lock(&q->lock);

  if (list_empty(&q->head)) {
    goto atomic_queue_pop_exit;
  } else {
    item = q->head.next;
    list_del(item);
  }

atomic_queue_pop_exit:
  mutex_unlock(&q->lock);
  return item;
}

void
atomic_queue_nint_init(struct atomic_queue_nint* q)
{
  list_init(&q->head);
}

void
atomic_queue_nint_push(struct atomic_queue_nint* q, struct list_head* item)
{
  bool intr_status = intr_lock();

  list_add_tail(item, &q->head);

  intr_unlock(intr_status);
}

struct list_head*
atomic_queue_nint_pop(struct atomic_queue_nint* q)
{
  struct list_head* item = NULL;
  bool intr_status = intr_lock();

  if (list_empty(&q->head)) {
    goto atomic_queue_nint_pop_exit;
  } else {
    item = q->head.next;
    list_del(item);
  }

atomic_queue_nint_pop_exit:
  intr_unlock(intr_status);
  return item;
}