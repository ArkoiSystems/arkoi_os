#include "lib/memory/vmm.h"

#include <stdint.h>

#include "arch/x86/idt/idt.h"
#include "lib/kmemory.h"
#include "lib/kstdio.h"
#include "lib/ksymbols.h"

#define PAGE_ENTRIES (1024U)
#define PAGE_SIZE (4096U)

static pde_t g_page_directories[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
static pte_t g_page_table[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

static uintptr_t virtual_to_physical(const uintptr_t virtual_address) {
    return virtual_address - KERNEL_VIRTUAL_BASE;
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

void vmm_init_paging() {
    pde_t* page_directory = (pde_t*)virtual_to_physical((uintptr_t)g_page_directories);
    kmemset(page_directory, 0, sizeof(g_page_directories));

    pte_t* page_table = (pte_t*)virtual_to_physical((uintptr_t)g_page_table);
    kmemset(page_table, 0, sizeof(g_page_table));

    // Identity map the first 4 MiB of memory for the kernel and boot region
    for (uint32_t index = 0; index < PAGE_ENTRIES; index++) {
        page_table[index].address = (index * PAGE_SIZE) >> 12;
        page_table[index].present = 1;
        page_table[index].rw = 1;
    }

    // Set the first PDE to point to our page table for identity mapping
    page_directory[0].address = ((uintptr_t)page_table) >> 12;
    page_directory[0].present = 1;
    page_directory[0].rw = 1;

    // Set the PDE for the kernel virtual base to point to the same page table as the first PDE (identity mapping)
    uint32_t pde_index = KERNEL_VIRTUAL_BASE >> 22;
    page_directory[pde_index].address = ((uintptr_t)page_table) >> 12;
    page_directory[pde_index].present = 1;
    page_directory[pde_index].rw = 1;

    load_page_directory(page_directory);
    enable_paging();
}

void vmm_initialize() {
    // Paging is already enabled during early bootstrap in C.
    isr_install(EXCEPTION_PAGE_FAULT, &vmm_page_fault_handler);
}

void vmm_page_fault_handler(const isr_frame_t* frame) {
    uint32_t error_code = frame->err_code;
    uint32_t faulting_address = frame->cr2;

    const char* reason = "?";
    if ((error_code & 0x1) == 0) {
        reason = "not present";
    } else if (error_code & 0x2) {
        reason = "write to read-only";
    } else {
        reason = "protection violation";
    }

    const char* mode = (error_code & 0x4) ? "user" : "kernel";
    const char* access_type = (error_code & 0x2) ? "write" : "read";

    kprintf("========================================\n");
    kprintf("PAGE FAULT!\n");
    kprintf("Faulting address: %x\n", faulting_address);
    kprintf("Error code: %x (reason: %s, mode: %s, access: %s)\n", error_code, reason, mode, access_type);
    kprintf("Instruction: %x\n", frame->eip);
    kprintf("========================================\n");
}
