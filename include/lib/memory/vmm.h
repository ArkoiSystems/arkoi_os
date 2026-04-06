#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#include "arch/x86/idt/idt.h"

/**
 * @brief Represents a page directory entry (PDE) in x86 paging.
 *
 * The PDE structure is defined according to the x86 architecture specification for 32-bit paging.
 * It contains various flags and the physical address of the page table it points to.
 */
typedef struct {
    uint32_t present : 1;        // Page is present in physical memory
    uint32_t rw : 1;             // Read/write (1 = readable & writable, 0 = read-only)
    uint32_t user : 1;           // User/supervisor (1 = user mode, 0 = kernel mode)
    uint32_t write_through : 1;  // Write-through (1 = write-through, 0 = write-back)
    uint32_t cache_disabled : 1; // Cache disabled (1 = no caching, 0 = cache enabled)
    uint32_t accessed : 1;       // Accessed (set by CPU on access, cleared by OS)
    uint32_t avl_1 : 1;          // Ignored by CPU
    uint32_t page_size : 1;      // Page size (1 = 4 MiB page, 0 = 4 KiB page)
    uint32_t avl_2 : 4;          // Ignored by CPU
    uint32_t address : 20;       // Physical address of page table (first 20bits, 4 KiB aligned)
} __attribute__((packed)) pde_t;

/**
 * @brief Represents a page table entry (PTE) in x86 paging.
 *
 * The PTE structure is defined according to the x86 architecture specification for 32-bit paging.
 * It contains various flags and the physical address of the page frame it points to.
 */
typedef struct {
    uint32_t present : 1;        // Page is present in physical memory
    uint32_t rw : 1;             // Read/write (1 = readable & writable, 0 = read-only)
    uint32_t user : 1;           // User/supervisor (1 = user mode, 0 = kernel mode)
    uint32_t write_through : 1;  // Write-through (1 = write-through, 0 = write-back)
    uint32_t cache_disabled : 1; // Cache disabled (1 = no caching, 0 = cache enabled)
    uint32_t accessed : 1;       // Accessed (set by CPU on access, cleared by OS)
    uint32_t dirty : 1;          // Dirty (set by CPU on write, cleared by OS)
    uint32_t pat : 1;            // Page Attribute Table index (used if PAT is supported, otherwise 0)
    uint32_t global : 1;         // Global page (1 = not flushed from TLB on CR3 reload)
    uint32_t avl : 3;            // Ignored by CPU
    uint32_t address : 20;       // Physical address of the page frame (first 20 bits, 4 KiB aligned)
} __attribute__((packed)) pte_t;

void vmm_init_paging();

void vmm_init();

void vmm_page_fault_handler(const isr_frame_t* frame);

#endif // VMM_H
