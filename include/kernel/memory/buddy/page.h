#pragma once

#include "kernel/memory/memory.h"
#include "kernel/utils/list_head.h"
#include "lib/types.h"

/**
 * @brief Physical memory page abstraction
 */
struct page
{
  u8 order;
  enum mem_type zone_type;

  struct list_head node;
  bool reserved;
  bool buddy;
};

/**
 * @brief Initialize the physical memory page abstraction
 */
void
init_page(void);

/**
 * @brief Get the index of the page
 *
 * @param page page to get the index of
 * @return size_t index of the page
 */
size_t
page_get_index(struct page* page);

bool
page_index_is_overflow(size_t index);

/**
 * @brief Get the page from the index
 *
 * @param index index of the page
 * @return struct page* page
 */
struct page*
page_get(size_t index);

/**
 * @brief Get the physical address of the page
 *
 * @param page page to get the physical address of
 * @return uintptr_t physical address of the page
 */
uintptr_t
page_get_phys(struct page* page);

struct page*
page_get_by_phys(uintptr_t phys);

/**
 * @brief Get the virtual address of the page (kernel only)
 *
 * @param page page to get the virtual address of
 * @return uintptr_t virtual address of the page
 */
uintptr_t
page_get_virt(struct page* page);

struct page*
page_get_by_virt(uintptr_t virt);