#include <stdint.h>

#include "arch/x86/boot/multiboot2.h"
#include "arch/x86/gdt/gdt.h"
#include "arch/x86/idt/idt.h"
#include "drivers/keyboard.h"
#include "drivers/pit.h"
#include "drivers/vga.h"
#include "lib/early_alloc.h"
#include "lib/kpanic.h"
#include "lib/kstdio.h"
#include "lib/ksymbols.h"
#include "lib/pmm.h"

#if defined(__linux__)
#error "This kernel is not meant to be compiled on Linux!"
#elif !defined(__i386__)
#error "This kernel needs to be compiled with a x86-elf compiler!"
#endif

void kernel_main(multiboot2_info_t* mb2_info) {
    vga_initialize();

    boot_info_t boot_info;
    multiboot2_parse_boot_info(mb2_info, &boot_info);
    multiboot2_print_boot_info(&boot_info);

    while (1);
}
