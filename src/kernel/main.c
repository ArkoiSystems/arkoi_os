#include <stdint.h>

#include "arch/x86/boot/multiboot2.h"
#include "arch/x86/gdt/gdt.h"
#include "arch/x86/idt/idt.h"
#include "drivers/keyboard.h"
#include "drivers/pit.h"
#include "drivers/vga.h"
#include "lib/kmalloc.h"
#include "lib/kpanic.h"
#include "lib/kstdio.h"
#include "lib/symbols.h"

#if defined(__linux__)
#error "This kernel is not meant to be compiled on Linux!"
#elif !defined(__i386__)
#error "This kernel needs to be compiled with a x86-elf compiler!"
#endif

void kernel_main(multiboot2_info_t* mb2_info) {
    vga_initialize();
    kprintf("VGA initialized!\n");

    // Parse the Multiboot2 info structure provided by the bootloader
    boot_info_t boot_info;
    multiboot2_parse_boot_info(mb2_info, &boot_info);

    kprintf("Booted using the \"%s\" boot loader\n", boot_info.name);
    kprintf("The command line is \"%s\"\n", boot_info.command_line);

    kprintf("RAM has a size of %d MB\n", multiboot2_memory_map_size(&boot_info.ram) / (1024 * 1024));
    for (size_t index = 0; index < boot_info.ram.count; index++) {
        boot_memory_region_t* region = &boot_info.ram.regions[index];

        // Cast the addresses to uint32_t as the kprintf doesnt support 64-bit values yet.
        uint32_t start_address = region->base_address;
        uint32_t end_address = region->base_address + region->length;
        uint32_t size = region->length / 1024;

        kprintf(" - RAM Region %d: %x - %x (%d KB)\n", index, start_address, end_address, size);
    }

    kprintf("There are %d reserved memory regions\n", boot_info.reserved.count);
    for (size_t index = 0; index < boot_info.reserved.count; index++) {
        boot_memory_region_t* region = &boot_info.reserved.regions[index];

        // Cast the addresses to uint32_t as the kprintf doesnt support 64-bit values yet.
        uint32_t start_address = region->base_address;
        uint32_t end_address = region->base_address + region->length;
        uint32_t size = region->length / 1024;

        kprintf(" - Reserved Region %d: %x - %x (%d KB)\n", index, start_address, end_address, size);
    }

    kprintf("There are %d modules loaded", boot_info.module_count);
    for (size_t index = 0; index < boot_info.module_count; index++) {
        boot_module_t* module = &boot_info.modules[index];

        uint32_t start_address = module->mod_start;
        uint32_t end_address = module->mod_end;
        uint32_t size = module->mod_end - module->mod_start;

        kprintf(" - Module %d: %x - %x (%d KB)\n", index, start_address, end_address, size / 1024);
    }

    while (1);

    // kmalloc_init();
    // kprintf("Heap initialized!\n");

    // gdt_initialize();
    // kprintf("GDT initialized!\n");

    // idt_initialize();
    // kprintf("IDT initialized!\n");

    // pit_initialize();
    // kprintf("PIT initialized!\n");

    // keyboard_initialize();
    // kprintf("Keyboard initialized!\n");

    // kprintf("\n");

    // kprintf("%s from my %d-bit kernel!\n", "Hello World", 32);
    // kprintf("This is VGA text mode, here is some %x to enjoy!\n", 0xDEADBEEF);
    // kprintf("Using %%c you can print out single characters%c\n", '!');
    // kprintf("All invalid formats just get printed out %42!\n");

    // kprintf("\n");

    // kprintf("Memory Section Map\n");
    // kprintf(" %s: %x - %x (%d KB)\n", "text", SYMBOL_START(text), SYMBOL_END(text), SYMBOL_SIZE(text) / 1024);
    // kprintf(" %s: %x - %x (%d KB)\n", "rodata", SYMBOL_START(rodata), SYMBOL_END(rodata), SYMBOL_SIZE(rodata) /
    // 1024); kprintf(" %s: %x - %x (%d KB)\n", "data", SYMBOL_START(data), SYMBOL_END(data), SYMBOL_SIZE(data) / 1024);
    // kprintf(" %s: %x - %x (%d KB)\n", "bss", SYMBOL_START(bss), SYMBOL_END(bss), SYMBOL_SIZE(bss) / 1024);
    // kprintf(" %s: %x - %x (%d KB)\n", "heap", SYMBOL_START(heap), SYMBOL_END(heap), SYMBOL_SIZE(heap) / 1024);

    // kprintf("\n");

    // void* ptr_1 = kmalloc(1);
    // kprintf("kmalloc(24) returned %x\n", ptr_1);

    // void* ptr_2 = kmalloc(120);
    // kprintf("kmalloc(120) returned %x\n", ptr_2);

    // void* ptr_3 = kmalloc(1);
    // kprintf("kmalloc(8) returned %x\n", ptr_3);

    // void* ptr_4 = kmalloc(56);
    // kprintf("kmalloc(56) returned %x\n", ptr_4);

    // void* ptr_5 = kmalloc(24);
    // kprintf("kmalloc(24) returned %x\n", ptr_5);

    // kfree(ptr_1);
    // kprintf("kfree(%x)\n", ptr_1);

    // kfree(ptr_2);
    // kprintf("kfree(%x)\n", ptr_2);

    // kfree(ptr_3);
    // kprintf("kfree(%x)\n", ptr_3);

    // kfree(ptr_4);
    // kprintf("kfree(%x)\n", ptr_4);

    // kfree(ptr_5);
    // kprintf("kfree(%x)\n", ptr_5);

    // while (1) {
    //     if (!keyboard_has_event()) {
    //         continue;
    //     }

    //     keyboard_event_t event;
    //     keyboard_get_event(&event);

    //     if (!event.is_pressed) {
    //         continue;
    //     }

    //     char ascii;

    //     size_t result = keyboard_scancode_to_ascii(&event, &ascii);
    //     if (result != 0) {
    //         continue;
    //     }

    //     vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    //     vga_put_char(ascii);
    // }
}
