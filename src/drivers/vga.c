#include "drivers/vga.h"

#include "lib/kio.h"
#include "lib/kstdio.h"

static uint16_t* g_vga_memory = (uint16_t*)VGA_MEMORY_ADDRESS;

static uint32_t g_row = 0, g_column = 0;
static uint8_t g_current_color = 0;

static uint16_t vga_entry(const char character, const uint8_t color) {
    return (uint16_t)character | (uint16_t)color << 8;
}

static void vga_scroll() {
    for (uint16_t y = 1; y < VGA_HEIGHT; y++) {
        for (uint16_t x = 0; x < VGA_WIDTH; x++) {
            const uint16_t original_index = y * VGA_WIDTH + x;
            const uint16_t new_index = (y - 1) * VGA_WIDTH + x;
            g_vga_memory[new_index] = g_vga_memory[original_index];
        }
    }

    for (uint16_t x = 0; x < VGA_WIDTH; x++) {
        const uint16_t last_row_index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        g_vga_memory[last_row_index] = vga_entry(' ', g_current_color);
    }

    vga_set_cursor(VGA_HEIGHT - 1, g_column);
}

void vga_init() {
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear();
}

void vga_clear() {
    for (uint16_t index = 0; index < VGA_WIDTH * VGA_HEIGHT; index++) {
        g_vga_memory[index] = vga_entry(' ', g_current_color);
    }

    vga_set_cursor(0, 0);
}

void vga_set_color(const vga_color_t foreground, const vga_color_t background) {
    g_current_color = (foreground | background << 4);
}

void vga_write_char(const char character) {
    if (character == '\n') {
        g_column = 0;
        if (++g_row >= VGA_HEIGHT) {
            vga_scroll();
        }
        return;
    }

    const uint16_t position = g_row * VGA_WIDTH + g_column;
    g_vga_memory[position] = vga_entry(character, g_current_color);

    if (++g_column >= VGA_WIDTH) {
        g_column = 0;

        if (++g_row >= VGA_HEIGHT) {
            vga_scroll();
        }
    }

    vga_set_cursor(g_row, g_column);
}

void vga_write_string(const char* data) {
    while (*data) {
        vga_write_char(*data);
        data++;
    }
}

void vga_set_cursor(uint16_t row, uint16_t column) {
    if (row >= VGA_HEIGHT || column >= VGA_WIDTH) return;

    g_column = column;
    g_row = row;

    const uint16_t position = g_row * VGA_WIDTH + g_column;
    outb(VGA_CRTC_INDEX_PORT, VGA_CRTC_CURSOR_LOW);
    outb(VGA_CRTC_DATA_PORT, (uint8_t)(position & 0xFF));
    outb(VGA_CRTC_INDEX_PORT, VGA_CRTC_CURSOR_HIGH);
    outb(VGA_CRTC_DATA_PORT, (uint8_t)((position >> 8) & 0xFF));
}

void vga_get_cursor(uint16_t* row, uint16_t* column) {
    *column = g_column;
    *row = g_row;
}
