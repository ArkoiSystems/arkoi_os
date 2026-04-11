#ifndef VGA_H
#define VGA_H

#include <stdint.h>

// VGA text-mode dimensions and memory address
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY_ADDRESS 0xB8000

// VGA I/O port definitions for cursor control
#define VGA_CRTC_INDEX_PORT 0x3D4
#define VGA_CRTC_DATA_PORT 0x3D5
#define VGA_CRTC_CURSOR_LOW 0x0F
#define VGA_CRTC_CURSOR_HIGH 0x0E

/**
 * @brief Enumeration of VGA color codes for text and background in text-mode.
 *
 * This enumeration defines the standard VGA color codes that can be used to set the foreground (text) and background
 * colors in the VGA text-mode buffer. Each color is represented by a unique integer value, which corresponds to the
 * color's code in the VGA hardware. These colors can be combined to create various visual effects in text output on the
 * screen.
 */
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
 * This function sets up the VGA text-mode buffer with default configurations. It resets the text and background colors
 * to a predefined default setting and clears the screen to ensure a clean start for text rendering.
 */
void vga_init();

/**
 * @brief Clears the VGA text-mode buffer and resets cursor position.
 *
 * This function fills the entire VGA text-mode buffer with spaces (' ') using the current color setting. It effectively
 * clears the screen. After clearing, the cursor position is reset to the top-left corner (row 0, column 0).
 *
 * @note This function depends on the current foreground and background color already set by vga_set_color.
 */
void vga_clear();

/**
 * @brief Sets the text and background color in the VGA text-mode buffer.
 *
 * This function updates the current color settings used for subsequent text output in the VGA text-mode buffer. The
 * foreground color specifies the text color, while the background color specifies the color behind the text.
 *
 * @param foreground The foreground (text) color to be applied.
 * @param background The background color to be applied.
 */
void vga_set_color(vga_color_t foreground, vga_color_t background);

/**
 * @brief Outputs a single character to the VGA text-mode buffer.
 *
 * This function places a character at the current cursor position on the VGA text-mode buffer. It handles special
 * characters such as newlines by moving the cursor to the start of the next line. If the cursor moves beyond the bounds
 * of the screen, the function ensures proper scrolling of the display.
 *
 * @param character The character to be written to the VGA text-mode buffer.
 */
void vga_write_char(char character);

/**
 * @brief Writes a null-terminated string to the VGA text-mode buffer.
 *
 * This function outputs each character of the provided string to the VGA text-mode buffer. It handles newlines by
 * moving the cursor to the start of the next line and ensures proper scrolling of the VGA display buffer when the
 * content exceeds the visible screen area.
 *
 * @param data A pointer to the null-terminated string to be written to the VGA text-mode buffer.
 */
void vga_write_string(const char* data);

/**
 * @brief Sets the cursor position in the VGA text-mode buffer.
 *
 * This function updates the current cursor position to the specified row and column. The cursor position is used for
 * subsequent character output in the VGA text-mode buffer. If the specified position is outside the bounds of the
 * screen, the function should handle it gracefully, either by clamping the values or by ignoring the request.
 *
 * @param row    The row index (0-based) where the cursor should be positioned.
 * @param column The column index (0-based) where the cursor should be positioned.
 */
void vga_set_cursor(uint16_t row, uint16_t column);

/**
 * @brief Retrieves the current cursor position in the VGA text-mode buffer.
 *
 * This function provides the current row and column indices of the cursor in the VGA text-mode buffer. The retrieved
 * values can be used for various purposes, such as saving the cursor position before performing operations that
 * may change it, or for implementing features that require knowledge of the current cursor location.
 *
 * @param row    A pointer to a variable where the current row index (0-based) of the cursor will be stored.
 * @param column A pointer to a variable where the current column index (0-based) of the cursor will be stored.
 */
void vga_get_cursor(uint16_t* row, uint16_t* column);

#endif // VGA_H
