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
    vga_init();

    boot_info_t boot_info;
    multiboot2_parse_boot_info(mb2_info, &boot_info);
    multiboot2_print_boot_info(&boot_info);

    pmm_t pmm;
    pmm_init(&pmm);

    boot_memory_region_t* current_ram = boot_info.ram_regions;
    while (current_ram != NULL) {
        uintptr_t start = current_ram->base_address;
        uintptr_t size = current_ram->length;

        pmm_add_region(&pmm, start, size);

        current_ram = current_ram->next;
    }

    pit_init();

    keyboard_init();

    serial_init_port(SERIAL_PORT_COM1, 115200U, SERIAL_DATA_BITS_8, SERIAL_PARITY_NONE, SERIAL_STOP_BITS_1);
    serial_write_string(SERIAL_PORT_COM1, "Serial port COM1 initialized successfully!\n");

    // Trigger a page fault for testing
    *(uintptr_t*)(8 * 1024 * 1024) = 100;

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
