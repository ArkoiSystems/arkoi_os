#ifndef KMALLOC_H
#define KMALLOC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "lib/kassert.h"

#define MAX_ORDER (20U) // 2^20 = 1048576 bytes (1 MiB)
#define MIN_ORDER (10U) // 2^10 = 1024 bytes

typedef struct block {
    struct block* next;
    uint8_t order;
    bool is_free;
} block_t;

STATIC_ASSERT((sizeof(block_t) & (sizeof(block_t) - 1)) == 0, BLOCK_HEADER_SIZE_MUST_BE_POWER_OF_2);
STATIC_ASSERT((sizeof(block_t) == 8), BLOCK_HEADER_SIZE_MUST_BE_8_BYTES);

void kmalloc_init();

void* kmalloc(size_t size);

void kfree(void* ptr);

#endif // KMALLOC_H
