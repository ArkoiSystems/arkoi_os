#include "lib/pmm.h"

#include <stdint.h>

#include "lib/early_alloc.h"
#include "lib/kpanic.h"
#include "lib/kstdio.h"
#include "lib/ksymbols.h"

#define FREE_LISTS(order) (free_lists[REL_ORDER(order)])
static block_meta_t* free_lists[NUM_ORDERS];

static block_meta_t* block_table;
static size_t block_table_len;

static bool size_to_order(const size_t size, size_t* result) {
    size_t order = MIN_ORDER;
    size_t block_size = BLOCK_SIZE(order);

    // Find the smallest order such that the block size can accommodate the requested size.
    while (block_size < size && order <= MAX_ORDER) {
        order++;
        block_size <<= 1;
    }

    if (order > MAX_ORDER) {
        return false;
    }

    *result = order;
    return true;
}

static size_t block_index_from_address(const uintptr_t address) {
    if (address < SYMBOL_START(heap) || address >= SYMBOL_END(heap)) {
        KPANIC("Block address %x is outside of heap bounds", address);
    }

    if ((address & (BLOCK_SIZE(MIN_ORDER) - 1U)) != 0U) {
        KPANIC("Block address %x is not page aligned", address);
    }

    uintptr_t offset = address - SYMBOL_START(heap);
    return (size_t)(offset >> MIN_ORDER);
}

static block_meta_t* block_meta_from_address(const uintptr_t address) {
    size_t index = block_index_from_address(address);

    if (index >= block_table_len) {
        KPANIC("Block index %d from address %x exceeds block table length %d", index, address, block_table_len);
    }

    return &block_table[index];
}

static block_meta_t* get_buddy(const block_meta_t* block, const size_t order) {
    // Calculate the buddy's address by XORing the block's address with the size of the block for the given order.
    uintptr_t buddy_address = block->address ^ BLOCK_SIZE(order);
    if (buddy_address < SYMBOL_START(heap) || buddy_address >= SYMBOL_END(heap)) {
        KPANIC("Calculated buddy address %x is outside of heap bounds", buddy_address);
    }

    return block_meta_from_address(buddy_address);
}

static block_meta_t** find_block_slot(block_meta_t** head, block_meta_t* target) {
    while (*head && *head != target) {
        head = &(*head)->next;
    }

    return head;
}

static void remove_from_list(block_meta_t* block, const int order) {
    block_meta_t** head = &FREE_LISTS(order);
    block_meta_t** slot = find_block_slot(head, block);

    if (*slot == NULL) {
        KPANIC("Block %x not found in free list for order %d", block->address, order);
    }

    *slot = block->next;
    block->next = NULL;
}

static void add_to_list(block_meta_t* block, const int order) {
    block->next = FREE_LISTS(order);
    FREE_LISTS(order) = block;
}

void pmm_init() {
    if (SYMBOL_SIZE(heap) < BLOCK_SIZE(MAX_ORDER)) {
        KPANIC("Heap size %d is too small for allocator (min %d bytes)", SYMBOL_SIZE(heap), BLOCK_SIZE(MAX_ORDER));
    }

    if ((SYMBOL_START(heap) & (BLOCK_SIZE(MAX_ORDER) - 1U)) != 0U) {
        KPANIC("Heap with start address %x is not properly aligned for buddy allocator", SYMBOL_START(heap));
    }

    if ((SYMBOL_SIZE(heap) & (BLOCK_SIZE(MIN_ORDER) - 1U)) != 0U) {
        KPANIC("Heap size %d is not page aligned", SYMBOL_SIZE(heap));
    }

    if ((SYMBOL_SIZE(heap) & (BLOCK_SIZE(MAX_ORDER) - 1U)) != 0U) {
        KPANIC("Heap size %d is not aligned to max-order blocks", SYMBOL_SIZE(heap));
    }

    block_table_len = (size_t)(SYMBOL_SIZE(heap) >> MIN_ORDER);
    block_table = (block_meta_t*)early_alloc(block_table_len * sizeof(block_meta_t));

    for (size_t order = MIN_ORDER; order <= MAX_ORDER; order++) {
        FREE_LISTS(order) = NULL;
    }

    for (size_t i = 0; i < block_table_len; i++) {
        block_table[i].next = NULL;
        block_table[i].address = SYMBOL_START(heap) + (i << MIN_ORDER);
        block_table[i].order = 0;
        block_table[i].is_free = false;
        block_table[i].is_present = false;
    }

    // Seed one max-order block for each max-order region in the heap.
    size_t max_blocks = (size_t)(SYMBOL_SIZE(heap) >> MAX_ORDER);
    for (size_t i = 0; i < max_blocks; i++) {
        uintptr_t block_address = SYMBOL_START(heap) + ((uintptr_t)i << MAX_ORDER);
        block_meta_t* block = block_meta_from_address(block_address);

        block->order = MAX_ORDER;
        block->is_free = true;
        block->is_present = true;
        add_to_list(block, MAX_ORDER);
    }
}

void* pmm_alloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    size_t target_order = 0;
    if (!size_to_order(size, &target_order)) {
        KPANIC("Allocation size %d is too large to be handled by the buddy allocator", size);
        return NULL;
    }

    // Find the first available block of the required order or higher
    size_t current_order = target_order;
    while (current_order <= MAX_ORDER) {
        if (FREE_LISTS(current_order) != NULL) {
            break;
        }

        current_order++;
    }

    // If we reached the end of the free lists without finding a suitable block, allocation fails
    if (current_order > MAX_ORDER) {
        KPANIC("No blocks available for order %d or higher", target_order);
        return NULL;
    }

    block_meta_t* target_block = FREE_LISTS(current_order);
    remove_from_list(target_block, (int)current_order);

    while (current_order > target_order) {
        current_order--;

        uintptr_t buddy_address = target_block->address + BLOCK_SIZE(current_order);
        block_meta_t* buddy_block = block_meta_from_address(buddy_address);

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

void pmm_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    uintptr_t start_address = (uintptr_t)ptr;

    if ((start_address & (BLOCK_SIZE(MIN_ORDER) - 1U)) != 0U) {
        KPANIC("Attempted to free a pointer %x that is not properly aligned", start_address);
    }

    if (start_address < SYMBOL_START(heap) || start_address >= SYMBOL_END(heap)) {
        KPANIC("Attempted to free a pointer %x that is outside of the heap bounds", start_address);
    }

    block_meta_t* block = block_meta_from_address(start_address);

    if (!block->is_present) {
        KPANIC("Attempted to free pointer %x that does not reference a live allocation", start_address);
    }

    if (block->order < MIN_ORDER || block->order > MAX_ORDER) {
        KPANIC("Attempted to free pointer %x with invalid block order %d", start_address, block->order);
    }

    if (block->is_free) {
        KPANIC("Double free detected for pointer %x", start_address);
    }

    // Mark the block as free before attempting to coalesce with its buddy
    block->is_free = true;

    size_t order = block->order;
    while (order < MAX_ORDER) {
        block_meta_t* buddy_block = get_buddy(block, order);

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
