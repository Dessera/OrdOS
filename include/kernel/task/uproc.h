#pragma once

/**
 * @brief User process entry point
 */
typedef void (*uproc_entry_t)(void);

/**
 * @brief Create a user process
 *
 * @param name Name of the process
 * @param entry Entry point of the process
 * @return struct task* Pointer to the task structure of the created process
 */
struct task*
uproc_create(char* name, uproc_entry_t entry);