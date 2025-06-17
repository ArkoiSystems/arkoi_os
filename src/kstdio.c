#include "kstdio.h"

#include <stdarg.h>

#include "vga.h"

static const char *HEX_DIGITS = "0123456789ABCDEF";
static const uint32_t BUFFER_SIZE = 11;

static void print_dec(int32_t value) {
    if (value == 0) {
        vga_put_char('0');
        return;
    }

    if (value < 0) {
        vga_put_char('-');
        value = -value;
    }

    char buffer[BUFFER_SIZE];
    int index = 0;

    while (value && index < BUFFER_SIZE - 1) {
        buffer[index] = '0' + (char) (value % 10);
        value /= 10;
        index++;
    }

    while (index--) vga_put_char(buffer[index]);
}

static void print_hex(const uint32_t value) {
    vga_write("0x");

    for (int shift = 28; shift >= 0; shift -= 4) {
        const uint8_t digit = (value >> shift) & 0xF;
        vga_put_char(HEX_DIGITS[digit]);
    }
}

void kprintf(const char *message, ...) {
    va_list args;
    va_start(args, message);

    for (const char *start = message; *start; start++) {
        if (*start != '%') {
            vga_put_char(*start);
            continue;
        }

        switch (*(++start)) {
            case 'd': {
                print_dec(va_arg(args, int32_t));
                break;
            }
            case 'x': {
                print_hex(va_arg(args, uint32_t));
                break;
            }
            case 's': {
                vga_write(va_arg(args, char *));
                break;
            }
            case 'c': {
                vga_put_char(va_arg(args, int));
                break;
            }
            case '%': {
                vga_put_char('%');
                break;
            }
            default: {
                vga_put_char('%');
                vga_put_char(*start);
                break;
            }
        }
    }

    va_end(args);
}
