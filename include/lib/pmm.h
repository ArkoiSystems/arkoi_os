#ifndef PMM_H
#define PMM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_ORDER (20U)                        // 2^20 = 1048576 bytes (1 MiB)
#define MIN_ORDER (12U)                        // 2^12 = 4096 bytes (4 KiB, the size of a page)
#define REL_ORDER(order) ((order) - MIN_ORDER) // Convert an order to an index in the free lists array
#define NUM_ORDERS (MAX_ORDER - MIN_ORDER + 1) // Total number of orders we support

#define BLOCK_SIZE(order) (1U << (order)) // Calculate the block size for a given order (2^order)

typedef struct block_meta {
    struct block_meta* next;
    uintptr_t address;
    uint8_t order;
    bool is_free;
    bool is_present;
} block_meta_t;

void pmm_init();

void* pmm_alloc(size_t size);

void pmm_free(void* ptr);

#endif // PMM_H
