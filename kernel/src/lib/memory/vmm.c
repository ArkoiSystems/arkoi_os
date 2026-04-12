#include "lib/memory/vmm.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "arch/x86/idt/idt.h"
#include "lib/kmemory.h"
#include "lib/kpanic.h"
#include "lib/kstdio.h"
#include "lib/ksymbols.h"
#include "lib/memory/pmm.h"

#define PAGE_ENTRIES (1024U)
#define PAGE_SIZE (4096U)

static pde_t g_page_directories[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
static pte_t g_identity_table[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

static uintptr_t virtual_to_physical(const uintptr_t virtual_address) {
    return virtual_address - KERNEL_VIRTUAL_BASE;
}

static uintptr_t physical_to_virtual(const uintptr_t physical_address) {
    return physical_address + KERNEL_VIRTUAL_BASE;
}

static void enable_paging() {
    // Read CR0 to get current value
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));

    // Set the PG bit (bit 31) to enable paging
    cr0 |= 0x80000000;

    // Write back to CR0 to enable paging
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}

static void load_page_directory(const pde_t* pd_physical) {
    // Load the page directory physical address into CR3
    __asm__ volatile("mov %0, %%cr3" : : "r"(pd_physical));
}

static void flush_tlb_entry(uintptr_t virtual_address) {
    // Invalidate the TLB entry for the given virtual address using the INVLPG instruction
    __asm__ volatile("invlpg (%0)" : : "r"(virtual_address) : "memory");
}

static pte_t* get_or_create_pte(uintptr_t virtual_address, vmm_flag_t flags) {
    uint32_t pde_index = (virtual_address >> 22) & 0x3FF; // Upper 10 bits
    uint32_t pte_index = (virtual_address >> 12) & 0x3FF; // Middle 10 bits

    pde_t* directory_entry = &g_page_directories[pde_index];
    if (directory_entry->present) {
        // Get the page table from physical address
        uintptr_t pt_physical = ((uintptr_t)directory_entry->address) << 12;
        uintptr_t pt_virtual = physical_to_virtual(pt_physical);

        pte_t* page_table = (pte_t*)pt_virtual;
        return &page_table[pte_index];
    }

    // Allocate physical memory for the page table (1 page = 4KiB, enough for 1024 entries)
    void* physical_page = pmm_alloc_pages(1);
    if (physical_page == NULL) {
        return NULL;
    }

    uintptr_t pt_physical = (uintptr_t)physical_page;
    uintptr_t pt_virtual = physical_to_virtual(pt_physical);
    kmemset((void*)pt_virtual, 0, PAGE_SIZE);

    // Set up the PDE to point to the new page table
    directory_entry->address = pt_physical >> 12;
    directory_entry->present = true;
    directory_entry->rw = (flags & VMM_FLAG_RW) != 0;
    directory_entry->user = (flags & VMM_FLAG_USER) != 0;
    directory_entry->write_through = (flags & VMM_FLAG_WRITE_THROUGH) != 0;
    directory_entry->cache_disabled = (flags & VMM_FLAG_CACHE_DISABLED) != 0;

    pte_t* page_table = (pte_t*)pt_virtual;
    return &page_table[pte_index];
}

void vmm_preinit() {
    // IMPORTANT: We are running with paging disabled at this point, so we must use physical addresses to set up the
    //            page directory and tables. That's why we use the virtual_to_physical function to get the physical
    //            addresses of our static page directory and identity table.

    pde_t* pd_physical = (pde_t*)virtual_to_physical((uintptr_t)g_page_directories);
    kmemset(pd_physical, 0, sizeof(g_page_directories));

    pte_t* identity_physical = (pte_t*)virtual_to_physical((uintptr_t)g_identity_table);
    kmemset(identity_physical, 0, sizeof(g_identity_table));

    // Identity map the first 4 MiB of memory for the kernel and boot region
    for (uint32_t index = 0; index < PAGE_ENTRIES; index++) {
        identity_physical[index].address = (index * PAGE_SIZE) >> 12;
        identity_physical[index].present = true;
        identity_physical[index].rw = true;
    }

    // Set the first PDE to point to our page table for identity mapping
    pd_physical[0].address = ((uintptr_t)identity_physical) >> 12;
    pd_physical[0].present = true;
    pd_physical[0].rw = true;

    // Set the PDE for the kernel virtual base to point to the same page table as the first PDE (identity mapping)
    uint32_t pde_index = KERNEL_VIRTUAL_BASE >> 22;
    pd_physical[pde_index].address = ((uintptr_t)identity_physical) >> 12;
    pd_physical[pde_index].present = true;
    pd_physical[pde_index].rw = true;

    load_page_directory(pd_physical);
    enable_paging();
}

void vmm_init() {
    // Install the page fault handler
    isr_install(EXCEPTION_PAGE_FAULT, &vmm_page_fault_handler);
}

bool vmm_map_page(uintptr_t virtual_address, uintptr_t physical_address, vmm_flag_t flags) {
    if ((virtual_address % PAGE_SIZE) != 0 || (physical_address % PAGE_SIZE) != 0) {
        return false;
    }

    pte_t* entry = get_or_create_pte(virtual_address, flags);
    if (entry == NULL) {
        return false;
    }

    if (entry->present) {
        return false;
    }

    // Set up the PTE with the physical address and flags
    entry->address = physical_address >> 12;
    entry->present = (flags & VMM_FLAG_PRESENT) != 0;
    entry->rw = (flags & VMM_FLAG_RW) != 0;
    entry->user = (flags & VMM_FLAG_USER) != 0;
    entry->write_through = (flags & VMM_FLAG_WRITE_THROUGH) != 0;
    entry->cache_disabled = (flags & VMM_FLAG_CACHE_DISABLED) != 0;
    entry->global = (flags & VMM_FLAG_GLOBAL) != 0;

    // Invalidate TLB entry for this page
    flush_tlb_entry(virtual_address);

    return true;
}

bool vmm_unmap_page(uintptr_t virtual_address) {
    if ((virtual_address % PAGE_SIZE) != 0) {
        return false;
    }

    uint32_t pde_index = (virtual_address >> 22) & 0x3FF;
    uint32_t pte_index = (virtual_address >> 12) & 0x3FF;

    pde_t* pde = &g_page_directories[pde_index];
    if (!pde->present) {
        return false;
    }

    uintptr_t pt_physical = ((uintptr_t)pde->address) << 12;
    uintptr_t pt_virtual = physical_to_virtual(pt_physical);

    pte_t* page_table = (pte_t*)pt_virtual;
    pte_t* entry = &page_table[pte_index];

    if (!entry->present) {
        return false;
    }

    // Clear the PTE to unmap the page
    kmemset(entry, 0, sizeof(pte_t));

    // Invalidate TLB entry
    flush_tlb_entry(virtual_address);

    return true;
}

void vmm_page_fault_handler(const isr_frame_t* frame) {
    uint32_t faulting_address = frame->cr2;
    uint32_t error_code = frame->err_code;

    if ((error_code & 0x1) == 0) {
        // Align the faulting address down to the nearest page boundary
        uintptr_t aligned_address = kalign_down(faulting_address, PAGE_SIZE);

        // Allocate a new physical page to map for this faulting address
        void* physical_page = pmm_alloc_pages(1);
        if (physical_page == NULL) {
            KPANIC("Out of memory in page fault handler!");
        }

        // If we fail to map the page, free the allocated physical page and panic
        if (!vmm_map_page(aligned_address, (uintptr_t)physical_page, VMM_FLAG_RW | VMM_FLAG_PRESENT)) {
            pmm_free(physical_page);
            KPANIC("Failed to map page in page fault handler!");
        }

        // Zero out the newly mapped page and return to the faulting code
        kmemset((void*)aligned_address, 0, PAGE_SIZE);
        return;
    } else {
        KPANIC("Unhandled page fault!");
    }
}
