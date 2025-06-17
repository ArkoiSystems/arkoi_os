#include "kstdio.h"
#include "vga.h"

void kernel_main(void) {
	vga_initialize();

	kprintf("%s from my %d-bit kernel!\n", "Hello World", 32);
	kprintf("This is VGA text mode, here is some %x to enjoy!\n", 0xDEADBEEF);
	kprintf("Using %%c you can print out single characters%c\n", '!');
	kprintf("All invalid formats just get printed out %42!");
}