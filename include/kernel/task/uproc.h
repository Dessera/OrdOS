#pragma once

typedef void (*uproc_entry_t)(void);

/**
 * @brief Create a user process
 *
 * @param name Name of the process
 * @param entry Entry point of the process
 */
void
uproc_create(char* name, uproc_entry_t entry);