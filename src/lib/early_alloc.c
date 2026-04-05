#include "lib/early_alloc.h"

#include <stdbool.h>
#include <stdint.h>

#include "lib/kpanic.h"
#include "lib/kstdio.h"
#include "lib/ksymbols.h"

static uintptr_t CURSOR = SYMBOL_START(early_heap);
static uintptr_t END = SYMBOL_END(early_heap);

void* early_alloc(size_t size) {
    uintptr_t current_cursor = CURSOR;
    uintptr_t next_cursor = current_cursor + size;

    if (next_cursor > END) {
        KPANIC("Early boot allocator exhausted while allocating %d bytes", size);
    }

    CURSOR = next_cursor;
    return (void*)current_cursor;
}
