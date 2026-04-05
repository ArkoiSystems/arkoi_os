#include "lib/memory/pmm.h"

#include <stdint.h>

#include "lib/kmemory.h"
#include "lib/kpanic.h"
#include "lib/kstdio.h"
#include "lib/memory/emm.h"

static pmm_block_meta_t* g_free_lists[NUM_ORDERS];
static pmm_block_meta_t* g_pmm_table;
static size_t g_table_length;

static bool g_pmm_initialized;
static uintptr_t g_pmm_start;
static uintptr_t g_pmm_end;

#define FREE_LISTS(order) (g_free_lists[REL_ORDER(order)])

static bool size_to_order(const size_t size, size_t* result) {
    size_t current_order = 0U;

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

static size_t block_index_from_address(const uintptr_t address) {
    if (address < g_pmm_start || address >= g_pmm_end) {
        KPANIC("Block address %x is outside of PMM arena bounds", address);
    }

    if ((address & (PAGE_SIZE - 1U)) != 0U) {
        KPANIC("Block address %x is not block aligned", address);
    }

    uintptr_t offset = address - g_pmm_start;
    return (size_t)(offset / PAGE_SIZE);
}

static pmm_block_meta_t* block_meta_from_address(const uintptr_t address) {
    size_t index = block_index_from_address(address);

    if (index >= g_table_length) {
        KPANIC("Block index %d from address %x exceeds table length %d", index, address, g_table_length);
    }

    return &g_pmm_table[index];
}

static pmm_block_meta_t* get_buddy_block(const pmm_block_meta_t* block, const size_t order) {
    uintptr_t buddy_address = block->address ^ BLOCK_SIZE(order);

    if (buddy_address < g_pmm_start || buddy_address >= g_pmm_end) {
        KPANIC("Calculated buddy address %x is outside of PMM arena bounds", buddy_address);
    }

    return block_meta_from_address(buddy_address);
}

static pmm_block_meta_t** find_block_slot(pmm_block_meta_t** head, pmm_block_meta_t* target) {
    while (*head && *head != target) {
        head = &(*head)->next;
    }

    return head;
}

static void remove_from_list(pmm_block_meta_t* block, const int order) {
    pmm_block_meta_t** head = &g_free_lists[order];

    pmm_block_meta_t** slot = find_block_slot(head, block);
    if (*slot == NULL) {
        KPANIC("Block %x not found in free list for order %d", block->address, order);
    }

    *slot = block->next;
    block->next = NULL;
}

static void add_to_list(pmm_block_meta_t* block, const int order) {
    block->next = g_free_lists[order];
    g_free_lists[order] = block;
}

bool pmm_init_from_range(uintptr_t arena_start, size_t arena_size) {
    if (g_pmm_initialized) {
        KPANIC("Attempted to initialize PMM multiple times", 0);
    }

    if (arena_size < BLOCK_SIZE(MAX_ORDER)) {
        KPANIC("Arena size %lu is smaller than maximum block size %lu", arena_size, BLOCK_SIZE(MAX_ORDER));
    }

    if ((arena_start & (BLOCK_SIZE(MAX_ORDER) - 1U)) != 0U) {
        KPANIC("Arena start address %x is not block aligned", arena_start);
    }

    if ((arena_size & (PAGE_SIZE - 1U)) != 0U) {
        KPANIC("Arena size %lu is not a multiple of the minimum block size %lu", arena_size, PAGE_SIZE);
    }

    if ((arena_size & (BLOCK_SIZE(MAX_ORDER) - 1U)) != 0U) {
        KPANIC("Arena size %lu is not a multiple of the maximum block size %lu", arena_size, BLOCK_SIZE(MAX_ORDER));
    }

    g_pmm_initialized = true;

    g_pmm_start = arena_start;
    g_pmm_end = arena_start + arena_size;

    g_table_length = arena_size / PAGE_SIZE;

    size_t meta_size = g_table_length * sizeof(pmm_block_meta_t);
    g_pmm_table = (pmm_block_meta_t*)emm_alloc(meta_size);
    kprintf("Allocated %d KB of metadata for the PMM\n", meta_size / 1024);

    for (size_t order = 0U; order <= MAX_ORDER; order++) {
        g_free_lists[order] = NULL;
    }

    for (size_t index = 0; index < g_table_length; index++) {
        g_pmm_table[index].next = NULL;
        g_pmm_table[index].address = g_pmm_start + (index * PAGE_SIZE);
        g_pmm_table[index].order = 0U;
        g_pmm_table[index].is_free = false;
        g_pmm_table[index].is_present = false;
    }

    size_t max_blocks = arena_size / BLOCK_SIZE(MAX_ORDER);
    for (size_t index = 0; index < max_blocks; index++) {
        uintptr_t block_address = g_pmm_start + ((uintptr_t)index * BLOCK_SIZE(MAX_ORDER));
        pmm_block_meta_t* block = block_meta_from_address(block_address);

        block->order = MAX_ORDER;
        block->is_free = true;
        block->is_present = true;

        add_to_list(block, MAX_ORDER);
    }

    return true;
}

bool pmm_init_from_memory_map(const boot_memory_region_t* ram_regions) {
    uintptr_t best_start = 0;
    size_t best_size = 0;

    const boot_memory_region_t* current = ram_regions;
    while (current != NULL) {
        uint64_t region_start64 = current->base_address;
        uint64_t region_end64 = current->base_address + current->length;

        if (region_end64 < region_start64) {
            current = current->next;
            continue;
        }

        if (region_start64 > UINTPTR_MAX) {
            current = current->next;
            continue;
        }

        uint64_t max_addressable_end = (uint64_t)UINTPTR_MAX + 1ULL;
        if (region_end64 > max_addressable_end) {
            region_end64 = max_addressable_end;
        }

        uintptr_t region_start = (uintptr_t)region_start64;
        uintptr_t region_end = (uintptr_t)region_end64;

        uintptr_t candidate_start = kalign_up(region_start, BLOCK_SIZE(MAX_ORDER));
        if (candidate_start >= region_end) {
            current = current->next;
            continue;
        }

        uintptr_t candidate_end = kalign_down(region_end, BLOCK_SIZE(MAX_ORDER));
        if (candidate_end <= candidate_start) {
            current = current->next;
            continue;
        }

        size_t candidate_size = candidate_end - candidate_start;
        if (candidate_size > best_size) {
            best_start = candidate_start;
            best_size = candidate_size;
        }

        current = current->next;
    }

    if (best_size == 0) {
        return false;
    }

    return pmm_init_from_range(best_start, best_size);
}

void* pmm_alloc_page(size_t pages) {
    if (!g_pmm_initialized) {
        KPANIC("PMM used before initialization", 0);
    }

    if (pages == 0) {
        return NULL;
    }

    return pmm_alloc_size(pages * PAGE_SIZE);
}

void* pmm_alloc_order(size_t order) {
    if (!g_pmm_initialized) {
        KPANIC("PMM used before initialization", 0);
    }

    if (order > MAX_ORDER) {
        KPANIC("Invalid target order %d for allocation", order);
    }

    size_t current_order = order;
    while (current_order <= MAX_ORDER) {
        if (g_free_lists[current_order] != NULL) {
            break;
        }

        current_order++;
    }

    if (current_order > MAX_ORDER) {
        KPANIC("No blocks available for order %d or higher", order);
        return NULL;
    }

    pmm_block_meta_t* target_block = g_free_lists[current_order];
    remove_from_list(target_block, (int)current_order);

    while (current_order > order) {
        current_order--;

        uintptr_t buddy_address = target_block->address + BLOCK_SIZE(current_order);
        pmm_block_meta_t* buddy_block = block_meta_from_address(buddy_address);

        target_block->order = (uint8_t)current_order;
        target_block->is_present = true;

        buddy_block->order = (uint8_t)current_order;
        buddy_block->is_free = true;
        buddy_block->is_present = true;
        buddy_block->next = NULL;

        add_to_list(buddy_block, (int)current_order);
    }

    target_block->is_free = false;
    target_block->is_present = true;
    target_block->next = NULL;

    return (void*)target_block->address;
}

void* pmm_alloc_size(size_t size) {
    if (!g_pmm_initialized) {
        KPANIC("PMM used before initialization", 0);
    }

    if (size == 0) {
        return NULL;
    }

    size_t target_order = 0;
    if (!size_to_order(size, &target_order)) {
        KPANIC("Allocation size %d is too large to be handled by the buddy allocator", size);
    }

    return pmm_alloc_order(target_order);
}

void pmm_free(void* ptr) {
    if (!g_pmm_initialized) {
        KPANIC("PMM used before initialization", 0);
    }

    if (ptr == NULL) {
        return;
    }

    uintptr_t start_address = (uintptr_t)ptr;

    if ((start_address & (PAGE_SIZE - 1U)) != 0U) {
        KPANIC("Attempted to free a pointer %x that is not properly aligned", start_address);
    }

    if (start_address < g_pmm_start || start_address >= g_pmm_end) {
        KPANIC("Attempted to free a pointer %x that is outside of PMM arena bounds", start_address);
    }

    pmm_block_meta_t* block = block_meta_from_address(start_address);

    if (!block->is_present) {
        KPANIC("Attempted to free pointer %x that does not reference a live allocation", start_address);
    }

    if (block->order > MAX_ORDER) {
        KPANIC("Attempted to free pointer %x with invalid block order %d", start_address, block->order);
    }

    if (block->is_free) {
        KPANIC("Double free detected for pointer %x", start_address);
    }

    block->is_free = true;

    size_t order = block->order;
    while (order < MAX_ORDER) {
        pmm_block_meta_t* buddy_block = get_buddy_block(block, order);

        if (!buddy_block->is_present || !buddy_block->is_free || buddy_block->order != order) {
            break;
        }

        remove_from_list(buddy_block, (int)order);

        if (buddy_block->address < block->address) {
            block = buddy_block;
        }

        buddy_block->is_present = false;
        buddy_block->is_free = false;
        buddy_block->next = NULL;

        order++;
        block->order = (uint8_t)order;
        block->is_present = true;
        block->is_free = true;
    }

    add_to_list(block, (int)order);
}
