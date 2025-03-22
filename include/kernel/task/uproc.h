#pragma once

typedef void (*uproc_entry_t)(void);

void
uproc_create(char* name, uproc_entry_t entry);