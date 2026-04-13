#include "lib/memory/emm.h"

#include <stdbool.h>
#include <stdint.h>

#include "lib/kpanic.h"
#include "lib/ksymbols.h"

static uintptr_t g_cursor = SYMBOL_START(early_heap);
static uintptr_t g_end = SYMBOL_END(early_heap);

void* emm_alloc(size_t size) {
    uintptr_t current_cursor = g_cursor;
    uintptr_t next_cursor = current_cursor + size;

    if (next_cursor > g_end) {
        KPANIC("Early boot allocator exhausted while allocating %u bytes", size);
    }

    g_cursor = next_cursor;
    return (void*)current_cursor;
}
