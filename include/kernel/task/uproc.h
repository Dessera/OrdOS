#pragma once

typedef void (*uproc_entry_t)(void);

void
uproc_run(char* name, uproc_entry_t entry);