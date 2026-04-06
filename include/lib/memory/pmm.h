#ifndef PMM_H
#define PMM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "arch/x86/boot/multiboot2.h"

#define PAGE_SIZE (4096U)                               // 4 KiB, the size of a page (minimum block size)
#define BLOCK_SIZE(order) ((1U << (order)) * PAGE_SIZE) // Block size for a given order (2^order * PAGE_SIZE)
#define BLOCK_COUNT(order) (1U << (order))              // Number of blocks in a block of the given order

#define MAX_ORDER (10U)            // BLOCK_SIZE(10U) = 4 MiB
#define NUM_ORDERS (MAX_ORDER + 1) // Total number of orders we support

typedef struct pmm_block {
    uintptr_t address;
    uint8_t order;
    bool is_free;
    struct pmm_block* next;
} pmm_block_t;

typedef struct pmm_region {
    pmm_block_t* free_lists[NUM_ORDERS];
    pmm_block_t* block_pool;
    size_t block_pool_size;
    uintptr_t start;
    uintptr_t end;
    struct pmm_region* next;
} pmm_region_t;

typedef struct pmm {
    pmm_region_t* regions;
} pmm_t;

void pmm_initialize(pmm_t* pmm);

void pmm_add_region(pmm_t* pmm, uintptr_t start, uint32_t size);

void* pmm_alloc_order(pmm_t* pmm, uint8_t order);

void* pmm_alloc_pages(pmm_t* pmm, size_t num_pages);

void* pmm_alloc_size(pmm_t* pmm, size_t size);

void pmm_free(pmm_t* pmm, void* address);

#endif // PMM_H
