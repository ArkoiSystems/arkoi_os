#ifndef VMM_H
#define VMM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "arch/x86/idt/idt.h"

typedef enum {
    VMM_FLAG_NONE = 0,                /**< No flags, page is not present and has default permissions */
    VMM_FLAG_PRESENT = 1 << 0,        /**< Page is present in physical memory */
    VMM_FLAG_RW = 1 << 1,             /**< Read/write (1 = readable & writable, 0 = read-only) */
    VMM_FLAG_USER = 1 << 2,           /**< User/supervisor (1 = user mode, 0 = kernel mode) */
    VMM_FLAG_WRITE_THROUGH = 1 << 3,  /**< Write-through (1 = write-through, 0 = write-back) */
    VMM_FLAG_CACHE_DISABLED = 1 << 4, /**< Cache disabled (1 = no caching, 0 = cache enabled) */
    VMM_FLAG_GLOBAL = 1 << 8          /**< Global page (1 = not flushed from TLB on CR3 reload) */
} vmm_flag_t;

/**
 * @brief Represents a page directory entry (PDE) in x86 paging.
 *
 * The PDE structure is defined according to the x86 architecture specification for 32-bit paging.
 * It contains various flags and the physical address of the page table it points to.
 */
typedef struct {
    uint32_t present : 1;        /**< Page is present in physical memory */
    uint32_t rw : 1;             /**< Read/write (1 = readable & writable, 0 = read-only) */
    uint32_t user : 1;           /**< User/supervisor (1 = user mode, 0 = kernel mode) */
    uint32_t write_through : 1;  /**< Write-through (1 = write-through, 0 = write-back) */
    uint32_t cache_disabled : 1; /**< Cache disabled (1 = no caching, 0 = cache enabled) */
    uint32_t accessed : 1;       /**< Accessed (set by CPU on access, cleared by OS) */
    uint32_t avl_1 : 1;          /**< Ignored by CPU */
    uint32_t page_size : 1;      /**< Page size (1 = 4 MiB page, 0 = 4 KiB page) */
    uint32_t avl_2 : 4;          /**< Ignored by CPU */
    uint32_t address : 20;       /**< Physical address of page table (first 20bits, 4 KiB aligned) */
} __attribute__((packed)) pde_t;

/**
 * @brief Represents a page table entry (PTE) in x86 paging.
 *
 * The PTE structure is defined according to the x86 architecture specification for 32-bit paging.
 * It contains various flags and the physical address of the page frame it points to.
 */
typedef struct {
    uint32_t present : 1;        /**< Page is present in physical memory */
    uint32_t rw : 1;             /**< Read/write (1 = readable & writable, 0 = read-only) */
    uint32_t user : 1;           /**< User/supervisor (1 = user mode, 0 = kernel mode) */
    uint32_t write_through : 1;  /**< Write-through (1 = write-through, 0 = write-back) */
    uint32_t cache_disabled : 1; /**< Cache disabled (1 = no caching, 0 = cache enabled) */
    uint32_t accessed : 1;       /**< Accessed (set by CPU on access, cleared by OS) */
    uint32_t dirty : 1;          /**< Dirty (set by CPU on write, cleared by OS) */
    uint32_t pat : 1;            /**< Page Attribute Table index (used if PAT is supported, otherwise 0) */
    uint32_t global : 1;         /**< Global page (1 = not flushed from TLB on CR3 reload) */
    uint32_t avl : 3;            /**< Ignored by CPU */
    uint32_t address : 20;       /**< Physical address of the page frame (first 20 bits, 4 KiB aligned) */
} __attribute__((packed)) pte_t;

/**
 * @brief Pre-initializes the virtual memory manager (VMM) during early boot.
 *
 * This function sets up the initial page directory and identity mapping required for the kernel to operate before the
 * full VMM is initialized. As it also enables paging, it should be called at the appropriate point during the early
 * boot process. After this function is called, the kernel will be running with paging enabled, and the VMM can be fully
 * initialized with the PMM using `vmm_init`.
 */
void vmm_preinit();

/**
 * @brief Initializes the virtual memory manager (VMM) for the kernel.
 *
 * This function is responsible for setting up the virtual memory management system in the kernel.
 * It must be called after the physical memory manager (PMM) is initialized.
 */
void vmm_init();

/**
 * @brief Maps a virtual page to a physical page with specified flags.
 *
 * This function creates a mapping from the given virtual address to the given physical address.
 * If the page table for the virtual address does not exist, it will be created dynamically.
 *
 * @param virtual_address  The virtual address to map (should be page-aligned, i.e., a multiple of 4096).
 * @param physical_address The physical address to map to (should be page-aligned).
 * @param flags            The flags for the page mapping, specified as a combination of `vmm_flag_t` values.
 * @return true if the mapping was successful, false otherwise.
 */
bool vmm_map_page(uintptr_t virtual_address, uintptr_t physical_address, vmm_flag_t flags);

/**
 * @brief Unmaps a virtual page, removing the mapping.
 *
 * This function removes the mapping for a given virtual address. It does not free the page table
 * or the underlying physical page.
 *
 * @param virtual_address The virtual address to unmap.
 * @return true if the page was unmapped successfully, false if the page was not mapped.
 */
bool vmm_unmap_page(uintptr_t virtual_address);

/**
 * @brief Handles a page fault exception.
 *
 * This function is called when a page fault occurs, which happens when the CPU tries to access a page that is not
 * present in memory, or when there is a violation of the access permissions (e.g., writing to a read-only page). The
 * handler should determine the cause of the page fault and take appropriate action, such as loading the required page
 * into memory or terminating the offending process.
 *
 * @param frame A pointer to the interrupt frame containing the state of the CPU at the time of the page fault.
 */
void vmm_page_fault_handler(const isr_frame_t* frame);

#endif // VMM_H
