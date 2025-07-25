#include "drivers/keyboard.h"
#include "drivers/pit.h"
#include "drivers/vga.h"
#include "lib/symbols.h"
#include "lib/kstdio.h"
#include "gdt/gdt.h"
#include "idt/idt.h"

void kernel_main(void) {
	vga_initialize();
	kprintf("VGA initialized!\n");

	gdt_initialize();
	kprintf("GDT initialized!\n");

	idt_initialize();
	kprintf("IDT initialized!\n");

	pit_initialize();
	kprintf("PIT initialized!\n");

	keyboard_initialize();
	kprintf("Keyboard initialized!\n");

	kprintf("\n");

	kprintf("%s from my %d-bit kernel!\n", "Hello World", 32);
	kprintf("This is VGA text mode, here is some %x to enjoy!\n", 0xDEADBEEF);
	kprintf("Using %%c you can print out single characters%c\n", '!');
	kprintf("All invalid formats just get printed out %42!\n");

	kprintf("\n");

	kprintf("Memory Section Map\n");
	kprintf(" %s: %x - %x (%d bytes)\n", "text", SYMBOL_START(text), SYMBOL_END(text), SYMBOL_SIZE(text));
	kprintf(" %s: %x - %x (%d bytes)\n", "rodata", SYMBOL_START(rodata), SYMBOL_END(rodata), SYMBOL_SIZE(rodata));
	kprintf(" %s: %x - %x (%d bytes)\n", "data", SYMBOL_START(data), SYMBOL_END(data), SYMBOL_SIZE(data));
	kprintf(" %s: %x - %x (%d bytes)\n", "bss", SYMBOL_START(bss), SYMBOL_END(bss), SYMBOL_SIZE(bss));

	kprintf("\n");

	// ReSharper disable once CppDFAEndlessLoop
	while (1);
}