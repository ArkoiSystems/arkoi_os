#ifndef PMM_H
#define PMM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "arch/x86/boot/multiboot2.h"

#define PAGE_SIZE (4096U)                               // 4 KiB, the size of a page (minimum block size)
#define BLOCK_SIZE(order) ((1U << (order)) * PAGE_SIZE) // Block size for a given order (2^order * PAGE_SIZE)

#define MAX_ORDER (10U)            // BLOCK_SIZE(10U) = 4 MiB
#define NUM_ORDERS (MAX_ORDER + 1) // Total number of orders we support

typedef struct pmm_block_meta {
    struct pmm_block_meta* next;
    uintptr_t address;
    uint8_t order;
    bool is_present;
    bool is_free;
} pmm_block_meta_t;

bool pmm_init_from_range(uintptr_t arena_start, size_t arena_size);

bool pmm_init_from_memory_map(const boot_memory_region_t* ram_regions);

void* pmm_alloc_page(size_t pages);

void* pmm_alloc_order(size_t target_order);

void* pmm_alloc_size(size_t size);

void pmm_free(void* ptr);

#endif // PMM_H
