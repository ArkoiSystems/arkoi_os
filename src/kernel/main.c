#include <stdint.h>

#include "arch/x86/boot/multiboot2.h"
#include "arch/x86/gdt/gdt.h"
#include "arch/x86/idt/idt.h"
#include "drivers/keyboard.h"
#include "drivers/pit.h"
#include "drivers/vga.h"
#include "lib/kpanic.h"
#include "lib/kstdio.h"
#include "lib/ksymbols.h"
#include "lib/memory/emm.h"
#include "lib/memory/pmm.h"

#if defined(__linux__)
#error "This kernel is not meant to be compiled on Linux!"
#elif !defined(__i386__)
#error "This kernel needs to be compiled with a x86-elf compiler!"
#endif

void kernel_main(multiboot2_info_t* mb2_info) {
    vga_initialize();

    kprintf(
        "Early allocator start at %x with a size of %d KB\n", SYMBOL_START(early_heap), SYMBOL_SIZE(early_heap) / 1024);

    boot_info_t boot_info;
    multiboot2_parse_boot_info(mb2_info, &boot_info);
    multiboot2_print_boot_info(&boot_info);

    pmm_t pmm;
    pmm_initialize(&pmm);

    boot_memory_region_t* current_ram = boot_info.ram_regions;
    while (current_ram != NULL) {
        uintptr_t start = current_ram->base_address;
        uintptr_t size = current_ram->length;

        pmm_add_region(&pmm, start, size);

        current_ram = current_ram->next;
    }

    void* address_1 = pmm_alloc_order(&pmm, 2);
    kprintf("(1) Allocated 2 pages at address %x\n", address_1);

    void* address_2 = pmm_alloc_order(&pmm, 3);
    kprintf("(2) Allocated 3 pages at address %x\n", address_2);

    void* address_3 = pmm_alloc_order(&pmm, 2);
    kprintf("(3) Allocated 2 pages at address %x\n", address_3);

    void* address_4 = pmm_alloc_order(&pmm, 2);
    kprintf("(4) Allocated 2 pages at address %x\n", address_4);

    while (1);
}
