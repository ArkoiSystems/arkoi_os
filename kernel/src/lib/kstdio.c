#include "lib/kstdio.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "drivers/vga.h"
#include "lib/kcursor.h"
#include "lib/kstring.h"

static const char* HEX_DIGITS_UPPER = "0123456789ABCDEF";
static const char* HEX_DIGITS_LOWER = "0123456789abcdef";

static bool write_u32_dec(kcursor_t* cursor, uint32_t value) {
    if (cursor == NULL) {
        return false;
    }

    uint32_t temp = value;
    uint32_t divisor = 1;
    while (temp >= 10) {
        divisor *= 10;
        temp /= 10;
    }

    while (divisor != 0) {
        const uint32_t digit = value / divisor;
        const char digit_char = '0' + digit;

        if (!kcursor_write_byte(cursor, digit_char)) return false;

        value %= divisor;
        divisor /= 10;
    }

    return true;
}

static bool write_i32_dec(kcursor_t* cursor, int32_t value) {
    if (cursor == NULL) {
        return false;
    }

    if (value < 0) {
        if (!kcursor_write_byte(cursor, '-')) {
            return false;
        }

        // Convert through unsigned arithmetic so INT_MIN is handled safely.
        return write_u32_dec(cursor, 0U - (uint32_t)value);
    }

    return write_u32_dec(cursor, (uint32_t)value);
}

static bool write_hex_nibbles(
    kcursor_t* cursor, const uint32_t value, const uint8_t nibbles, const bool uppercase, const bool alternate_form) {
    if (cursor == NULL) return false;

    if (alternate_form) {
        if (!kcursor_write_byte(cursor, '0')) return false;
        if (!kcursor_write_byte(cursor, uppercase ? 'X' : 'x')) return false;
    }

    const char* hex_digits = uppercase ? HEX_DIGITS_UPPER : HEX_DIGITS_LOWER;
    for (int shift = (nibbles - 1) * 4; shift >= 0; shift -= 4) {
        const uint8_t digit = (value >> shift) & 0xF;
        if (!kcursor_write_byte(cursor, hex_digits[digit])) {
            return false;
        }
    }

    return true;
}

static size_t hex_digit_count(uint32_t value) {
    size_t count = 0;
    do {
        count++;
        value >>= 4;
    } while (value != 0);
    return count;
}

static bool write_u32_hex(kcursor_t* cursor, const uint32_t value, const bool uppercase, const bool alternate_form) {
    return write_hex_nibbles(cursor, value, hex_digit_count(value), uppercase, alternate_form);
}

static bool write_pointer(kcursor_t* cursor, const uintptr_t value) {
    return write_hex_nibbles(cursor, value, 8, false, true);
}

uint32_t kvsnprintf(char* buffer, const uint32_t size, const char* format, va_list args) {
    if (buffer == NULL || format == NULL || size == 0) {
        return 0;
    }

    kcursor_t cursor;
    if (!kcursor_init(&cursor, buffer, size - 1)) {
        return 0;
    }

    const size_t format_length = kstrlen(format);
    for (uint32_t index = 0; index < format_length; index++) {
        char current = format[index];

        // Encountering a non-format specifier will just print out the char.
        if (current != '%') {
            if (!kcursor_write_byte(&cursor, current)) {
                break;
            } else {
                continue;
            }
        }

        // Skip the percentage sign for further format processing. If it's a null terminator we're done.
        current = format[++index];
        if (current == '\0') break;

        // Behavior modifiers
        bool alternate_form = false;

        if (current == '#') {
            alternate_form = true;
            current = format[++index];
            if (current == '\0') break;
        }

        // Process the specifier character
        if (current == 'd' || current == 'i') {
            const int32_t value = va_arg(args, int);
            if (!write_i32_dec(&cursor, value)) break;
        } else if (current == 'u') {
            const uint32_t value = va_arg(args, uint32_t);
            if (!write_u32_dec(&cursor, value)) break;
        } else if (current == 'x' || current == 'X') {
            const uint32_t value = va_arg(args, uint32_t);
            if (!write_u32_hex(&cursor, value, current == 'X', alternate_form)) break;
        } else if (current == 'p') {
            const uintptr_t value = va_arg(args, uintptr_t);
            if (!write_pointer(&cursor, value)) break;
        } else if (current == 's') {
            const char* value = va_arg(args, char*);
            if (value == NULL) value = "NULL";

            const uint32_t length = kstrlen(value);
            if (!kcursor_write(&cursor, value, length)) break;
        } else if (current == 'c') {
            const char value = va_arg(args, int);
            if (!kcursor_write_byte(&cursor, value)) break;
        } else if (current == '%') {
            if (!kcursor_write_byte(&cursor, '%')) break;
        } else {
            if (!kcursor_write_byte(&cursor, '%')) break;
            if (!kcursor_write_byte(&cursor, current)) break;
        }
    }

    // We always reserve space for the null terminator, so we can safely write it here.
    const uint32_t written = kcursor_written(&cursor);
    buffer[written] = '\0';

    // Account for the null terminator in the return value to match the behavior of standard snprintf.
    return written + 1;
}

uint32_t ksnprintf(char* buffer, const uint32_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);

    const uint32_t written = kvsnprintf(buffer, size, format, args);

    va_end(args);
    return written;
}

uint32_t kvprintf(const char* format, va_list args) {
    static const uint32_t PRINTF_BUFFER_SIZE = 256;
    char buffer[PRINTF_BUFFER_SIZE];

    const uint32_t written = kvsnprintf(buffer, PRINTF_BUFFER_SIZE, format, args);
    if (written != 0) vga_write_string(buffer);

    return written;
}

uint32_t kprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    const uint32_t written = kvprintf(format, args);

    va_end(args);
    return written;
}
