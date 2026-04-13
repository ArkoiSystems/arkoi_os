#include "lib/kstdio.h"

#include <stdarg.h>
#include <stddef.h>

#include "drivers/vga.h"
#include "lib/kcursor.h"
#include "lib/kstring.h"

static const char* HEX_DIGITS = "0123456789ABCDEF";

static bool write_dec(kcursor_t* cursor, int32_t value) {
    if (cursor == NULL) {
        return false;
    }

    if (value < 0) {
        if (!kcursor_write_byte(cursor, '-')) return false;

        value = -value;
    }

    int32_t temp = value;
    int32_t divisor = 1;
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

static bool write_hex(kcursor_t* cursor, const uint32_t value) {
    if (cursor == NULL) return false;

    if (!kcursor_write_byte(cursor, '0')) return false;
    if (!kcursor_write_byte(cursor, 'x')) return false;

    for (int shift = 28; shift >= 0; shift -= 4) {
        const uint8_t digit = (value >> shift) & 0xF;
        if (!kcursor_write_byte(cursor, HEX_DIGITS[digit])) {
            return false;
        }
    }

    return true;
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

        // Process the specifier character
        if (current == 'd') {
            const int32_t value = va_arg(args, int);
            if (!write_dec(&cursor, value)) break;
        } else if (current == 'x') {
            const uint32_t value = va_arg(args, uint32_t);
            if (!write_hex(&cursor, value)) break;
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

    return written;
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
