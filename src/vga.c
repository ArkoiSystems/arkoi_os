#include "vga.h"

static uint16_t *VGA_MEMORY = (uint16_t *) VGA_MEMORY_ADDRESS;

static uint32_t ROW = 0, COLUMN = 0;
static uint8_t CURRENT_COLOR = 0;

static void vga_scroll(void) {
    for (uint16_t y = 1; y < VGA_HEIGHT; y++) {
        for (uint16_t x = 0; x < VGA_WIDTH; x++) {
            const uint16_t original_index = y * VGA_WIDTH + x;
            const uint16_t new_index = (y - 1) * VGA_WIDTH + x;
            VGA_MEMORY[new_index] = VGA_MEMORY[original_index];
        }
    }

    for (uint16_t x = 0; x < VGA_WIDTH; x++) {
        const uint16_t last_row_index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        VGA_MEMORY[last_row_index] = vga_entry(' ', CURRENT_COLOR);
    }

    ROW = VGA_HEIGHT - 1;
}

void vga_initialize(void) {
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear();
}

void vga_set_color(const vga_color_t foreground, const vga_color_t background) {
    CURRENT_COLOR = vga_entry_color(foreground, background);
}

void vga_put_char(const char character) {
    if (character == '\n') {
        COLUMN = 0;
        if (++ROW >= VGA_HEIGHT) vga_scroll();
        return;
    }

    const uint16_t index = ROW * VGA_WIDTH + COLUMN;
    VGA_MEMORY[index] = vga_entry(character, CURRENT_COLOR);

    if (++COLUMN >= VGA_WIDTH) {
        COLUMN = 0;
        if (++ROW >= VGA_HEIGHT) vga_scroll();
    }
}

void vga_write(const char *data) {
    while (*data) {
        vga_put_char(*data);
        data++;
    }
}

void vga_clear(void) {
    for (uint16_t index = 0; index < VGA_WIDTH * VGA_HEIGHT; index++) {
        VGA_MEMORY[index] = vga_entry(' ', CURRENT_COLOR);
    }

    COLUMN = 0;
    ROW = 0;
}
