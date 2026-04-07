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

/**
 * @brief Block structure for the physical memory manager.
 *
 * The `pmm_block_t` structure represents a block of memory that can be allocated or freed by the physical memory
 * manager. Each block has an address, an order (which determines its size), a flag indicating whether it is free or
 * allocated, and a pointer to the next block in the free list for its order. The order of a block determines its size,
 * with the block size defined as 2^order * PAGE_SIZE. For example, an order of 0 corresponds to a block size of 4 KiB,
 * an order of 1 corresponds to 8 KiB, and so on. The free blocks are organized into free lists based on their order,
 * allowing for efficient allocation and deallocation of memory blocks of varying sizes.
 */
typedef struct pmm_block {
    uintptr_t address;
    uint8_t order;
    bool is_free;
    struct pmm_block* next;
} pmm_block_t;

/**
 * @brief Memory region structure for the physical memory manager.
 *
 * The `pmm_region_t` structure represents a contiguous region of physical memory that is managed by the physical memory
 * manager. Each region maintains its own free lists for different block sizes (orders), as well as a pool of blocks
 * that can be allocated. The structure also keeps track of the starting and ending addresses of the region, and a
 * pointer to the next region in the linked list of memory regions managed by the PMM.
 */
typedef struct pmm_region {
    pmm_block_t* free_lists[NUM_ORDERS];
    pmm_block_t* block_pool;
    size_t block_pool_size;
    uintptr_t start;
    uintptr_t end;
    struct pmm_region* next;
} pmm_region_t;

/**
 * @brief Physical Memory Manager (PMM) structure.
 *
 * The `pmm_t` structure represents the physical memory manager, which is responsible for managing the allocation and
 * deallocation of physical memory in the system. It contains a linked list of memory regions, where each region manages
 * its own free lists for different block sizes (orders). The PMM provides functions to initialize the memory manager,
 * add memory regions, allocate blocks of memory based on size or order, and free allocated blocks back to the manager.
 */
typedef struct pmm {
    pmm_region_t* regions;
} pmm_t;

/**
 * @brief Initializes the physical memory manager.
 *
 * This function sets up the physical memory manager by initializing its internal data structures and preparing it to
 * manage physical memory. It should be called early in the system's initialization process, after the multiboot
 * information has been parsed and the available memory regions have been identified. The function will typically
 * initialize the free lists for each memory region, marking all blocks as free and ready for allocation.
 *
 * @param pmm A pointer to the physical memory manager instance that will be initialized. This should be a valid pointer
 *            to a `pmm_t` structure that has been allocated before calling this function.
 */
void pmm_init(pmm_t* pmm);

/**
 * @brief Adds a memory region to the physical memory manager.
 *
 * This function takes a pointer to the physical memory manager instance, the starting address of the memory region, and
 * the size of the region in bytes. It will create a new memory region structure, initialize it with the provided
 * information, and add it to the list of regions managed by the physical memory manager. The function will also
 * initialize the free lists for the new region, marking all blocks within the region as free and available for
 * allocation.
 *
 * @param pmm  A pointer to the physical memory manager instance.
 * @param start The starting address of the memory region to be added. This should be a physical address that is aligned
 *              to the page size (4 KiB), otherwise it will be rounded down to the nearest page boundary.
 * @param size The size of the memory region to be added, in bytes. This should be a positive integer and should ideally
 *             be a multiple of the page size for optimal performance, although the function will handle non-aligned
 *             sizes by rounding them up to the nearest page boundary as needed.
 */
void pmm_add_region(pmm_t* pmm, uintptr_t start, uint32_t size);

/**
 * @brief Allocates a block of memory of the specified order.
 *
 * This function will attempt to find a suitable block of memory that can accommodate the requested order. It will
 * search through the free lists of the physical memory manager, starting from the requested order and moving up to
 * higher orders if necessary, until it finds a suitable block. If a block is found, it will be marked as allocated and
 * returned to the caller. If no suitable block is found, the function will return NULL.
 *
 * @param pmm   A pointer to the physical memory manager instance.
 * @param order The order of the block to allocate. This should be a non-negative integer, where the block size is
 *              defined as 2^order * PAGE_SIZE. For example, an order of 0 corresponds to a block size of 4 KiB, an
 *              order of 1 corresponds to 8 KiB, and so on.
 * @return void* A pointer to the allocated memory, or NULL if the allocation fails due to insufficient free memory or
 *               fragmentation.
 */
void* pmm_alloc_order(pmm_t* pmm, uint8_t order);

/**
 * @brief Allocates a contiguous block of memory consisting of the specified number of pages.
 *
 * This function will attempt to find a suitable block of memory that can accommodate the requested number of pages. It
 * will search through the free lists of the physical memory manager, starting from the smallest order that can fit the
 * requested number of pages, and will return a pointer to the allocated block if successful. If no suitable block is
 * found, it will return NULL.
 *
 * @param pmm       A pointer to the physical memory manager instance.
 * @param num_pages The number of pages to allocate. This should be a positive integer, and the function will round it
 *                  up to the nearest block size as needed.
 * @return void* A pointer to the allocated memory, or NULL if the allocation fails due to insufficient free memory or
 *               fragmentation.
 */
void* pmm_alloc_pages(pmm_t* pmm, size_t num_pages);

/**
 * @brief Allocates a block of memory of the specified size.
 *
 * This function will attempt to find a suitable block of memory that can accommodate the requested size. It will search
 * through the free lists of the physical memory manager, starting from the smallest order that can fit the requested
 * size, and will return a pointer to the allocated block if successful. If no suitable block is found, it will return
 * NULL.
 *
 * @param pmm  A pointer to the physical memory manager instance.
 * @param size The size of the memory to allocate, in bytes. This can be any size, and the function will round it up to
 *             the nearest block size as needed.
 * @return void* A pointer to the allocated memory, or NULL if the allocation fails due to insufficient free memory or
 *               fragmentation.
 */
void* pmm_alloc_size(pmm_t* pmm, size_t size);

/**
 * @brief Frees a previously allocated block of memory back to the physical memory manager.
 *
 * This function takes a pointer to the physical memory manager and the address of the block to be freed. It will mark
 * the block as free and attempt to coalesce it with adjacent free blocks if possible, to reduce fragmentation and
 * improve the efficiency of future allocations.
 *
 * @param pmm     A pointer to the physical memory manager instance.
 * @param address The address of the block of memory to be freed. This should be a pointer that was previously returned
 *                by one of the allocation functions (`pmm_alloc_order`, `pmm_alloc_pages`, or `pmm_alloc_size`).
 */
void pmm_free(pmm_t* pmm, void* address);

#endif // PMM_H
