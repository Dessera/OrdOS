#pragma once

#include "lib/types.h"

#define LIST_ENTRY(ptr, type, member) CONTAINER_OF(ptr, type, member)

#define LIST_FOR_EACH(entry, head)                                             \
  for (entry = (head)->next; entry != (head); entry = entry->next)

/**
 * @brief List head structure, almost like Linux kernel list head
 */
struct list_head
{
  struct list_head *next, *prev;
};

/**
 * @brief Initialize a list head
 *
 * @param head List head to initialize
 */
void
list_init(struct list_head* head);

/**
 * @brief Insert a new entry between two known consecutive entries
 *
 * @param new New entry to be inserted
 * @param head List head to insert the new entry
 */
void
list_add(struct list_head* new, struct list_head* head);

/**
 * @brief Insert a new entry at the end of the list
 *
 * @param new New entry to be inserted
 * @param head List head to insert the new entry
 */
void
list_add_tail(struct list_head* new, struct list_head* head);

/**
 * @brief Remove a list entry from the list
 *
 * @param entry List entry to be removed
 */
void
list_del(struct list_head* entry);

/**
 * @brief Check if a list is empty
 *
 * @param head List head to check
 * @return true if the list is empty
 * @return false if the list is not empty
 */
bool
list_empty(struct list_head* head);

/**
 * @brief Remove and return the first entry of the list
 *
 * @param head List head to pop the entry from
 * @return struct list_head* First entry of the list
 */
struct list_head*
list_pop(struct list_head* head);

/**
 * @brief Find an entry in the list
 *
 * @param head List head to search in
 * @param entry Entry to find
 * @return ssize_t Index of the entry in the list, -1 if not found
 */
ssize_t
list_find(struct list_head* head, struct list_head* entry);
