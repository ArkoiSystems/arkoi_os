#include <stdint.h>

#include "arch/x86/boot/multiboot2.h"
#include "arch/x86/gdt/gdt.h"
#include "arch/x86/idt/idt.h"
#include "drivers/keyboard.h"
#include "drivers/pit.h"
#include "drivers/serial.h"
#include "drivers/vga.h"
#include "lib/kpanic.h"
#include "lib/kstdio.h"
#include "lib/ksymbols.h"
#include "lib/memory/emm.h"
#include "lib/memory/pmm.h"
#include "lib/memory/vmm.h"

#if defined(__linux__)
#error "This kernel is not meant to be compiled on Linux!"
#elif !defined(__i386__)
#error "This kernel needs to be compiled with a x86-elf compiler!"
#endif

void kernel_main(multiboot2_info_t* mb2_info) {
    serial_init();
    vga_init();

    boot_info_t boot_info;
    multiboot2_parse_boot_info(mb2_info, &boot_info);
    multiboot2_print_boot_info(&boot_info);

    pmm_init(&boot_info);
    vmm_init();

    SERIAL_PRINT(SERIAL_PORT_COM1, "Serial port COM1 initialized successfully!\n");

    // Trigger a page fault for testing
    uintptr_t test_address = 0x800000;
    *(uintptr_t*)(test_address) = 100;
    kprintf("Page fault test passed!\nThe value at %p is now: %u\n", test_address, *(uintptr_t*)(test_address));

    keyboard_init();

    while (1) {
        if (!keyboard_has_event()) {
            continue;
        }

        keyboard_event_t event;
        keyboard_get_event(&event);

        if (!event.is_pressed) {
            continue;
        }

        char ascii;

        bool result = keyboard_scancode_to_ascii(&event, &ascii);
        if (!result) {
            continue;
        }

        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_write_char(ascii);
    }
}
