#include "drivers/keyboard.h"
#include "drivers/pit.h"
#include "drivers/vga.h"
#include "lib/symbols.h"
#include "lib/kstdio.h"
#include "lib/kmalloc.h"
#include "arch/x86/gdt.h"
#include "arch/x86/idt.h"

void kernel_main(void) {
	vga_initialize();
	kprintf("VGA initialized!\n");

	kmalloc_init();
	kprintf("Heap initialized!\n");

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
	kprintf(" %s: %x - %x (%d bytes)\n", "heap", SYMBOL_START(heap), SYMBOL_END(heap), SYMBOL_SIZE(heap));

	kprintf("\n");

	void *ptr_1 = kmalloc(64);
	kprintf("kmalloc(64) returned %x\n", ptr_1);

	void *ptr_2 = kmalloc(128);
	kprintf("kmalloc(128) returned %x\n", ptr_2);

	kfree(ptr_1);
	kprintf("kfree(%x)\n", ptr_1);

	void *ptr_3 = kmalloc(64);
	kprintf("kmalloc(64) returned %x\n", ptr_3);

	void *ptr_4 = kmalloc(64);
	kprintf("kmalloc(64) returned %x\n", ptr_4);

	void *ptr_5 = kmalloc(64);
	kprintf("kmalloc(64) returned %x\n", ptr_5);

	// ReSharper disable once CppDFAEndlessLoop
	while (1);
}