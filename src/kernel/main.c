#include "drivers/keyboard.h"
#include "drivers/pit.h"
#include "drivers/vga.h"
#include "lib/kpanic.h"
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
	kprintf(" %s: %x - %x (%d KB)\n", "text", SYMBOL_START(text), SYMBOL_END(text), SYMBOL_SIZE(text) / 1024);
	kprintf(" %s: %x - %x (%d KB)\n", "rodata", SYMBOL_START(rodata), SYMBOL_END(rodata), SYMBOL_SIZE(rodata) / 1024);
	kprintf(" %s: %x - %x (%d KB)\n", "data", SYMBOL_START(data), SYMBOL_END(data), SYMBOL_SIZE(data) / 1024);
	kprintf(" %s: %x - %x (%d KB)\n", "bss", SYMBOL_START(bss), SYMBOL_END(bss), SYMBOL_SIZE(bss) / 1024);
	kprintf(" %s: %x - %x (%d KB)\n", "heap", SYMBOL_START(heap), SYMBOL_END(heap), SYMBOL_SIZE(heap) / 1024);

	kprintf("\n");

	void *ptr_1 = kmalloc(1);
	kprintf("kmalloc(24) returned %x\n", ptr_1);

	void *ptr_2 = kmalloc(120);
	kprintf("kmalloc(120) returned %x\n", ptr_2);

	void *ptr_3 = kmalloc(1);
	kprintf("kmalloc(8) returned %x\n", ptr_3);

	void *ptr_4 = kmalloc(56);
	kprintf("kmalloc(56) returned %x\n", ptr_4);

	void *ptr_5 = kmalloc(24);
	kprintf("kmalloc(24) returned %x\n", ptr_5);

	kfree(ptr_1);
	kprintf("kfree(%x)\n", ptr_1);

	kfree(ptr_2);
	kprintf("kfree(%x)\n", ptr_2);

	kfree(ptr_3);
	kprintf("kfree(%x)\n", ptr_3);

	kfree(ptr_4);
	kprintf("kfree(%x)\n", ptr_4);

	kfree(ptr_5);
	kprintf("kfree(%x)\n", ptr_5);

    while(1);
}