#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define VGA_WIDTH           80
#define VGA_HEIGHT          25
#define VGA_MEMORY_ADDRESS  0xB8000

typedef enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
} vga_color_t;

/**
 * @brief Initializes the VGA text-mode buffer for use.
 *
 * This function sets up the VGA text-mode buffer with default configurations.
 * It resets the text and background colors to a predefined default setting
 * and clears the screen to ensure a clean start for text rendering.
 */
void vga_initialize(void);

/**
 * @brief Sets the text and background color in the VGA text-mode buffer.
 *
 * This function updates the current color settings used for subsequent
 * text output in the VGA text-mode buffer. The foreground color specifies
 * the text color, while the background color specifies the color behind
 * the text.
 *
 * @param foreground The foreground (text) color to be applied.
 * @param background The background color to be applied.
 */
void vga_set_color(vga_color_t foreground, vga_color_t background);

/**
 * @brief Outputs a single character to the VGA text-mode buffer.
 *
 * This function places a character at the current cursor position on the
 * VGA text-mode buffer. It handles special characters such as newlines
 * by moving the cursor to the start of the next line. If the cursor
 * moves beyond the bounds of the screen, the function ensures proper
 * scrolling of the display.
 *
 * @param character The character to be written to the VGA text-mode buffer.
 */
void vga_put_char(char character);

/**
 * @brief Writes a null-terminated string to the VGA text-mode buffer.
 *
 * This function outputs each character of the provided string to the
 * VGA text-mode buffer. It handles newlines by moving the cursor to
 * the start of the next line and ensures proper scrolling of the VGA
 * display buffer when the content exceeds the visible screen area.
 *
 * @param data A pointer to the null-terminated string to be written
 *             to the VGA text-mode buffer.
 */
void vga_write(const char *data);

/**
 * @brief Clears the VGA text-mode buffer and resets cursor position.
 *
 * This function fills the entire VGA text-mode buffer with spaces (' ')
 * using the current color setting. It effectively clears the screen.
 * After clearing, the cursor position is reset to the top-left corner
 * (row 0, column 0).
 *
 * @note This function depends on the current foreground and background
 *       color already set by vga_set_color.
 */
void vga_clear(void);

/**
 * @brief Combines foreground and background VGA colors into a single byte encoding.
 *
 * This function is used to generate a VGA color byte containing both the
 * foreground and background colors. The foreground color occupies the
 * lower 4 bits, and the background color occupies the upper 4 bits of the byte.
 *
 * @param foreground The foreground color of type vga_color_t.
 * @param background The background color of type vga_color_t.
 *
 * @return A uint8_t value where the lower 4 bits represent the foreground color
 *         and the upper 4 bits represent the background color.
 */
static uint8_t vga_entry_color(const vga_color_t foreground, const vga_color_t background) {
    return foreground | background << 4;
}

/**
 * @brief Combines a character and VGA color into a single 16-bit encoded value.
 *
 * This function encodes a character and its corresponding VGA color into
 * a 16-bit value. The lower 8 bits represent the character, while the
 * upper 8 bits represent the color.
 *
 * @param character The character to encode, of type char.
 * @param color The foreground and background color encoded as a single byte, of type uint8_t.
 *
 * @return A uint16_t value where the lower 8 bits represent the character
 *         and the upper 8 bits represent the color.
 */
static uint16_t vga_entry(const char character, const uint8_t color) {
    return (uint16_t) character | (uint16_t) color << 8;
}

#endif //VGA_H
