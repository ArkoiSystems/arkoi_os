#include "lib/kmalloc.h"

#include "lib/kstdio.h"
#include "lib/symbols.h"
#include "lib/kpanic.h"
#include <stdint.h>

#define REL_ORDER(order) ((order) - MIN_ORDER)       // Convert an order to an index in the free lists array
#define NUM_ORDERS       (MAX_ORDER - MIN_ORDER + 1) // Total number of orders we support

static block_t *free_lists[NUM_ORDERS];

#define FREE_LISTS(order) (free_lists[REL_ORDER(order)]) // Access the free list for a given order
#define BLOCK_SIZE(order) (1U << (order))                // Calculate the block size for a given order (2^order)

static bool size_to_order(const size_t size, size_t *result) {
    size_t order = MIN_ORDER;
    size_t block_size = BLOCK_SIZE(order);

    // We need to account for the size of the block header (block_t) when determining the required order
    size_t required_size = size + sizeof(block_t);
    // Find the smallest order such that the block size can accommodate the requested size plus the block header
    while (block_size < required_size && order <= MAX_ORDER) {
        order++;
        block_size <<= 1;
    }

    if (order > MAX_ORDER) {
        return false;
    }

    *result = order;
    return true;
}

static block_t *get_buddy(const block_t *block, const size_t order) {
    uintptr_t block_address = (uintptr_t) block;

    // Calculate the buddy's address by XORing the block's address with the size of the block for the given order
    uintptr_t buddy_address = block_address ^ BLOCK_SIZE(order);
    if (buddy_address < SYMBOL_START(heap) || buddy_address >= SYMBOL_END(heap)) {
        KPANIC("Calculated buddy address %x is outside of heap bounds", buddy_address);
    }

    return (block_t *) buddy_address;
}

static block_t ** find_block_slot(block_t **head, block_t *target) {
    // Traverse the linked list starting from head to find the target block
    while(*head && *head != target) {
        head = &(*head)->next;
    }

    // Return a pointer to the pointer that references the target block (might be null if not found)
    return head;
}   

static void remove_from_list(block_t* block, const int order) {
    // Traverse the free list of the given order to find and remove the specified block
    block_t** head = &FREE_LISTS(order);
    block_t** slot = find_block_slot(head, block);

    // If the block is not found in the free list, this indicates a serious error in the allocator's state
    if(*slot == NULL) {
        KPANIC("Block %x not found in free list for order %d", block, order);
    }

    // Remove the block from the free list by updating the pointer to skip over it
    *slot = block->next;
}

static void add_to_list(block_t* block, const int order) {
    // Simple insertion at the head of the free list for the given order
    block->next = FREE_LISTS(order);
    FREE_LISTS(order) = block;
}

void kmalloc_init() {
    // Ensure the heap size is large enough to accommodate at least one block of the maximum order
    if(SYMBOL_SIZE(heap) < BLOCK_SIZE(MAX_ORDER)) {
        KPANIC("Heap size %d is too small for allocator (min %d bytes)", SYMBOL_SIZE(heap), BLOCK_SIZE(MAX_ORDER));
    }

    // Ensure the heap start address is properly aligned for the buddy allocator
    if ((SYMBOL_START(heap) & (BLOCK_SIZE(MAX_ORDER) - 1)) != 0) {
        KPANIC("Heap with start address %x is not properly aligned for buddy allocator", SYMBOL_START(heap));
    }
    
    // Initialize all free lists to NULL
    for (size_t order = MIN_ORDER; order <= MAX_ORDER; order++) {
        FREE_LISTS(order) = NULL;
    }

    // Create the initial block that represents the entire heap and add it to the free list for the maximum order
    block_t *initial_block = (block_t *) SYMBOL_START(heap);
    initial_block->order = MAX_ORDER;
    initial_block->next = NULL;
    initial_block->is_free = true;
    FREE_LISTS(MAX_ORDER) = initial_block;
}

void *kmalloc(size_t size) {
    // Find the smallest order that can accommodate the requested size
    size_t target_order = 0;
    if (!size_to_order(size, &target_order)) {
        KPANIC("Allocation size %d is too large to be handled by the buddy allocator", size);
        return NULL;
    }

    // Find the first available block of the required order or higher
    size_t current_order = target_order;
    while(current_order <= MAX_ORDER) {
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

    // Remove the block from the free list and split it down to the target order if necessary 
    block_t *target_block = FREE_LISTS(current_order);
    remove_from_list(target_block, current_order);

    while(current_order > target_order) {
        current_order--;

        // Split the block into two buddies
        size_t buddy_size = BLOCK_SIZE(current_order);
        block_t *buddy_block = (block_t *) ((uintptr_t) target_block + buddy_size);

        // Mark the buddy block as free
        buddy_block->is_free = true;

        // Set the order for both the target block and the buddy block
        target_block->order = current_order;
        buddy_block->order = current_order;

        // Add the buddy block to the free list for the current order
        add_to_list(buddy_block, current_order);
    }

    // Mark the target block as allocated
    target_block->is_free = false;

    // Return a pointer to the usable memory area of the allocated block (after the block header)
    uintptr_t block_address = (uintptr_t) target_block;
    uintptr_t start_address = block_address + sizeof(block_t);

    kprintf("Allocated block at %x with order %d for requested size %d\n", block_address, target_block->order, size);

    return (void *) start_address;
}

void kfree(void *ptr) {
    // If the pointer is NULL, we can simply ignore the free request as there is nothing to free
    if (ptr == NULL) {
        return;
    }

    uintptr_t start_address = (uintptr_t) ptr;

    // Validate that the pointer is properly aligned
    if (start_address & (sizeof(block_t) - 1)) {
        KPANIC("Attempted to free a pointer %x that is not properly aligned", start_address);
    }

    // Validate that the pointer is within the bounds of the heap
    if (start_address < SYMBOL_START(heap) || start_address >= SYMBOL_END(heap)) {
        KPANIC("Attempted to free a pointer %x that is outside of the heap bounds", start_address);
    }

    // As the pointer returned by kmalloc points to the usable memory area (starts after the block header), we need
    // to calculate the address of the block header
    uintptr_t block_address = start_address - sizeof(block_t);
    block_t *block = (block_t *) block_address;

    if (block->order < MIN_ORDER || block->order > MAX_ORDER) {
        KPANIC("Attempted to free pointer %x with invalid block order %d", start_address, block->order);
    }

    if (block->is_free) {
        KPANIC("Double free detected for pointer %x", start_address);
    }

    // Mark the block as free before attempting to coalesce with its buddy
    block->is_free = true;

    size_t order = block->order;
    while(order < MAX_ORDER) {
        // Calculate the buddy block for the current block and order
        block_t *buddy_block = get_buddy(block, order);

        // If the buddy block is not free or is of a different order, we cannot coalesce and must stop
        if(!buddy_block->is_free || buddy_block->order != order) {
            break;
        }

        // Remove the buddy block from the free list as we are going to coalesce it with the current block
        remove_from_list(buddy_block, order);

        // Determine the lower address between the current block and its buddy to be the new block after coalescing
        if (buddy_block < block) {
            block = buddy_block;
        }

        // Increase the order of the coalesced block to reflect that it now represents a larger block of memory
        order++;
        block->order = order;
    }

    // Add the (potentially coalesced) block back to the free list for its order
    add_to_list(block, order);
}