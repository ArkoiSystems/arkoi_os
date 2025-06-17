#include "vga.h"

void kernel_main(void) {
	vga_initialize();

	vga_write("Hello World from my 32-bit kernel!\n");
	vga_write("This is VGA text mode.\n");
}