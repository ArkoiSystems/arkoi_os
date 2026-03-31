#include "drivers/keyboard.h"

#include "drivers/vga.h"
#include "arch/x86/idt.h"
#include "arch/x86/pic.h"
#include "lib/kio.h"

char scancode_to_ascii[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
    '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, '*', 0, ' ',
};

int is_key_released(const uint8_t scancode) {
    return scancode & 0x80;
}

void keyboard_initialize() {
    irq_install(1, &keyboard_handler);
    pic_clear_mask(1);
}

void keyboard_handler([[maybe_unused]] const isr_frame_t *frame) {
    const uint8_t scancode = inb(0x60);

    const int released = is_key_released(scancode);
    if (released) return;

    const char ascii = scancode_to_ascii[scancode];
    if (ascii == 0) return;

    vga_put_char(ascii);
}