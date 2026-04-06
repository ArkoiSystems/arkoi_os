#include "lib/memory/vmm.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "arch/x86/idt/idt.h"
#include "lib/kmemory.h"
#include "lib/kstdio.h"
#include "lib/ksymbols.h"

#define PAGE_DIRECTORY_ENTRIES (1024U)
#define PAGE_TABLE_ENTRIES (1024U)
#define PAGE_SIZE (4096U)

static pde_t g_page_directories[PAGE_DIRECTORY_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
static pte_t g_page_table[PAGE_TABLE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

static void load_page_directory(pde_t* pd_physical) {
    // Load the page directory physical address into CR3
    __asm__ volatile("mov %0, %%cr3" : : "r"(pd_physical));
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

void setup_identity_paging() {
    for (size_t index = 0; index < PAGE_TABLE_ENTRIES; index++) {
        pte_t* entry = &g_page_table[index];
        entry->address = (index * PAGE_SIZE) >> 12; // Virtual address = physical address
        entry->present = true;
        entry->rw = true;
    }

    g_page_directories[0].address = ((uintptr_t)g_page_table) >> 12;
    g_page_directories[0].present = true;
    g_page_directories[0].rw = true;
}

void vmm_initialize() {
    kmemset(g_page_directories, 0, sizeof(g_page_directories));
    kmemset(g_page_table, 0, sizeof(g_page_table));

    // Install page fault handler before enabling paging
    isr_install(EXCEPTION_PAGE_FAULT, &vmm_page_fault_handler);

    setup_identity_paging();

    load_page_directory(g_page_directories);
    enable_paging();
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
