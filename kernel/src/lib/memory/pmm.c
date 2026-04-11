#include "lib/memory/pmm.h"

#include <stdint.h>

#include "lib/kmemory.h"
#include "lib/kpanic.h"
#include "lib/memory/emm.h"

static bool size_to_order(const size_t size, uint8_t* result) {
    if (result == NULL) {
        return false;
    }

    uint8_t current_order = 0U;

    size_t block_size = BLOCK_SIZE(current_order);
    while (block_size < size && current_order <= MAX_ORDER) {
        current_order++;
        block_size <<= 1;
    }

    if (current_order <= MAX_ORDER) {
        *result = current_order;
        return true;
    }

    return false;
}

static pmm_region_t* find_region_by_address(const pmm_t* pmm, const uintptr_t address) {
    if (pmm == NULL) {
        return NULL;
    }

    pmm_region_t* current = pmm->regions;
    while (current) {
        if (address >= current->start && address < current->end) {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

static pmm_block_t* find_block_by_address(const pmm_region_t* region, const uintptr_t address) {
    if (region == NULL) {
        return NULL;
    }

    if (address < region->start || address >= region->end) {
        return NULL;
    }

    if ((address & (PAGE_SIZE - 1U)) != 0U) {
        return NULL;
    }

    uintptr_t offset = address - region->start;
    size_t block_index = offset / PAGE_SIZE;

    if (block_index >= region->block_pool_size) {
        return NULL;
    }

    return &region->block_pool[block_index];
}

static pmm_region_t* find_region_for_order(const pmm_t* pmm, const uint8_t order) {
    if (pmm == NULL) {
        return NULL;
    }

    pmm_region_t* current = pmm->regions;
    while (current) {
        uint8_t current_order = order;
        while (current_order <= MAX_ORDER) {
            if (current->free_lists[current_order]) {
                return current;
            }

            current_order++;
        }

        current = current->next;
    }

    return NULL;
}

static pmm_block_t* get_buddy_block(const pmm_block_t* block, const pmm_region_t* region, const uint8_t order) {
    if (block == NULL || region == NULL) {
        return NULL;
    }

    uintptr_t region_relative = block->address - region->start;
    uintptr_t buddy_relative = region_relative ^ BLOCK_SIZE(order);
    uintptr_t buddy_address = region->start + buddy_relative;

    if (buddy_address < region->start || buddy_address >= region->end) {
        return NULL;
    }

    return find_block_by_address(region, buddy_address);
}

static bool remove_from_region_freelist(pmm_block_t* block, pmm_region_t* region, const uint8_t order) {
    if (block == NULL || region == NULL) {
        return false;
    }

    pmm_block_t** slot = &region->free_lists[order];
    while (*slot && *slot != block) {
        slot = &(*slot)->next;
    }

    if (*slot == NULL) {
        return false;
    }

    *slot = block->next;
    block->next = NULL;

    return true;
}

static void add_to_region_freelist(pmm_block_t* block, pmm_region_t* region, const uint8_t order) {
    block->next = region->free_lists[order];
    region->free_lists[order] = block;
}

void pmm_init(pmm_t* pmm) {
    pmm->regions = NULL;
}

void pmm_add_region(pmm_t* pmm, uintptr_t start, uint32_t size) {
    if (pmm == NULL) {
        KPANIC("Attempted to add memory region with a NULL pointer");
    }

    uintptr_t original_start = start;
    uintptr_t original_end = original_start + (uintptr_t)size;

    if (original_start >= original_end) {
        KPANIC("Memory region overflows address space (start=%x, size=%x)", original_start, size);
    }

    uintptr_t aligned_start = kalign_up(original_start, PAGE_SIZE);
    uintptr_t aligned_end = kalign_down(original_end, PAGE_SIZE);

    // Check if the aligned region is valid and can accommodate at least one block
    if (aligned_end <= aligned_start) {
        return;
    }

    size_t aligned_size = aligned_end - aligned_start;

    pmm_region_t* region = (pmm_region_t*)emm_alloc(sizeof(pmm_region_t));
    region->start = aligned_start;
    region->end = aligned_end;
    region->next = NULL;

    for (size_t index = 0; index < NUM_ORDERS; index++) {
        region->free_lists[index] = NULL;
    }

    region->block_pool_size = aligned_size / PAGE_SIZE;

    size_t blockpool_size = region->block_pool_size * sizeof(pmm_block_t);
    region->block_pool = (pmm_block_t*)emm_alloc(blockpool_size);

    for (size_t index = 0; index < region->block_pool_size; index++) {
        uintptr_t address = region->start + index * PAGE_SIZE;

        region->block_pool[index].address = address;
        region->block_pool[index].order = 0;
        region->block_pool[index].is_free = true;
        region->block_pool[index].next = NULL;
    }

    size_t offset = 0;
    while (offset < region->block_pool_size) {
        uint8_t biggest_order = 0U;

        while (biggest_order < MAX_ORDER) {
            size_t next_block_count = BLOCK_COUNT(biggest_order + 1);

            if (offset % next_block_count != 0) break;
            if (offset + next_block_count > region->block_pool_size) break;

            biggest_order++;
        }

        uintptr_t address = region->start + offset * PAGE_SIZE;

        pmm_block_t* block = find_block_by_address(region, address);
        block->order = biggest_order;
        block->is_free = true;
        add_to_region_freelist(block, region, biggest_order);

        offset += BLOCK_COUNT(biggest_order);
    }

    // Add the new region to the end of the linked list of regions
    pmm_region_t** head = &pmm->regions;
    while (*head) {
        head = &(*head)->next;
    }
    *head = region;
}

void* pmm_alloc_order(pmm_t* pmm, const uint8_t order) {
    if (pmm == NULL) {
        KPANIC("Attempted to allocate memory with a NULL pointer");
    }

    if (order > MAX_ORDER) {
        KPANIC("Invalid target order %d for allocation", order);
    }

    pmm_region_t* region = find_region_for_order(pmm, order);
    if (!region) {
        KPANIC("No free blocks available for order %d", order);
    }

    uint8_t current_order = order;
    while (current_order <= MAX_ORDER) {
        if (region->free_lists[current_order] != NULL) {
            break;
        }

        current_order++;
    }

    if (current_order > MAX_ORDER) {
        KPANIC("No blocks available for order %d or higher", order);
    }

    pmm_block_t* target_block = region->free_lists[current_order];
    if (!remove_from_region_freelist(target_block, region, current_order)) {
        KPANIC("Failed to remove block from free list");
    }

    target_block->is_free = false;

    while (current_order > order) {
        current_order--;

        uintptr_t buddy_address = target_block->address + BLOCK_SIZE(current_order);
        pmm_block_t* buddy_block = find_block_by_address(region, buddy_address);

        target_block->order = current_order;
        buddy_block->order = current_order;

        buddy_block->is_free = true;
        buddy_block->next = NULL;

        add_to_region_freelist(buddy_block, region, current_order);
    }

    target_block->next = NULL;

    return (void*)target_block->address;
}

void* pmm_alloc_pages(pmm_t* pmm, size_t num_pages) {
    if (pmm == NULL) {
        KPANIC("Attempted to allocate memory with a NULL pointer");
    }

    if (num_pages == 0) {
        return NULL;
    }

    if (num_pages > (SIZE_MAX / PAGE_SIZE)) {
        KPANIC("Requested page count %x causes size overflow", num_pages);
    }

    return pmm_alloc_size(pmm, num_pages * PAGE_SIZE);
}

void* pmm_alloc_size(pmm_t* pmm, const size_t size) {
    if (pmm == NULL) {
        KPANIC("Attempted to allocate memory with a NULL pointer");
    }

    if (size == 0) {
        return NULL;
    }

    uint8_t order;
    if (!size_to_order(size, &order)) {
        KPANIC("Requested allocation size %x is too large to be handled by the buddy allocator", size);
    }

    return pmm_alloc_order(pmm, order);
}

void pmm_free(pmm_t* pmm, void* address) {
    if (pmm == NULL) {
        KPANIC("Attempted to free memory with invalid argument");
    }

    if (address == NULL) {
        return;
    }

    uintptr_t start_address = (uintptr_t)address;
    if ((start_address & (PAGE_SIZE - 1U)) != 0U) {
        KPANIC("Attempted to free a pointer %x that is not properly aligned", start_address);
    }

    pmm_region_t* region = find_region_by_address(pmm, start_address);
    if (!region) {
        KPANIC("Attempted to free memory at address %x which does not belong to any region", start_address);
    }

    pmm_block_t* block = find_block_by_address(region, start_address);
    if (!block) {
        KPANIC("Attempted to free memory at address %x which does not belong to any block", start_address);
    }

    if (block->is_free) {
        KPANIC("Double free detected for pointer %x", start_address);
    }

    if (block->order > MAX_ORDER) {
        KPANIC("Attempted to free pointer %x with invalid block order %d", start_address, block->order);
    }

    if ((start_address - region->start) % BLOCK_SIZE(block->order) != 0) {
        KPANIC("Attempted to free non-block-aligned pointer %x", start_address);
    }

    block->is_free = true;

    uint8_t order = block->order;
    while (order < MAX_ORDER) {
        pmm_block_t* buddy_block = get_buddy_block(block, region, order);
        if (!buddy_block) {
            break;
        }

        if (!buddy_block->is_free || buddy_block->order != order) {
            break;
        }

        if (!remove_from_region_freelist(buddy_block, region, order)) {
            KPANIC("Failed to remove buddy block from free list during coalescing");
        }

        if (buddy_block->address < block->address) {
            block = buddy_block;
        }

        buddy_block->is_free = false;
        buddy_block->next = NULL;

        order++;
        block->order = order;
        block->is_free = true;
    }

    add_to_region_freelist(block, region, order);
}
