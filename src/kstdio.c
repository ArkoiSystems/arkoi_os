#include "kstdio.h"

#include <stdarg.h>
#include <stddef.h>

#include "kstring.h"
#include "vga.h"

static const char *HEX_DIGITS = "0123456789ABCDEF";

static uint32_t write_buffer(char *buffer, const uint32_t size, const char *value, const uint32_t length) {
    if (size == 0) return 0;

    // Check if the message fits into the buffer if not return 0.
    if (length >= size - 1) return 0;

    // Copy the message into the buffer.
    memmove(buffer, value, length);

    return length;
}

static uint32_t write_dec(char *buffer, const uint32_t size, int32_t value) {
    if (size == 0) return 0;

    // -2^(32 - 1) ... 2^(32 - 1) - 1
    // -2147483648 ... 2147483647
    // The maximum number of characters needed (including the negative sign) is 11.
    static const uint32_t SIGNED_BUFFER_SIZE = 11;
    char output[SIGNED_BUFFER_SIZE];
    uint32_t position = 0;

    if (value < 0) {
        output[position++] = '-';
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
        output[position++] = (char) ('0' + digit);
        value %= divisor;
        divisor /= 10;
    }

    return write_buffer(buffer, size, output, position);
}

static uint32_t write_hex(char *buffer, const uint32_t size, const uint32_t value) {
    if (size == 0) return 0;

    // 0xFFFFFFFF is the maximum number to display, thus only needing 10 characters.
    static const uint32_t HEX_BUFFER_SIZE = 10;
    char output[HEX_BUFFER_SIZE];
    uint32_t position = 0;

    output[position++] = '0';
    output[position++] = 'x';

    for (int shift = 28; shift >= 0; shift -= 4) {
        const uint8_t digit = (value >> shift) & 0xF;
        output[position++] = HEX_DIGITS[digit];
    }

    return write_buffer(buffer, size, output, position);
}

uint32_t vsnprintf(char *buffer, const uint32_t size, const char *format, va_list args) {
    if (size == 0) return 0;

    // Ensures that there is always room for the amount of character to write and a null terminator.
#define ENOUGH_SLOTS_FREE(amount) ((position + amount) < (size - 1))

    const uint32_t format_length = strlen(format);
    uint32_t position = 0;

    for (int index = 0; index < format_length; index++) {
        char current = format[index];

        // Encountering a non-format specifier will just print out the char.
        if (current != '%') {
            if (!ENOUGH_SLOTS_FREE(1)) goto cleanup;
            buffer[position++] = current;
            continue;
        }

        // Skip the percentage sign for further format processing. If it's a null terminator
        // we're done.
        current = format[++index];
        if (current == '\0') goto cleanup;

        // Process the specifier character
        switch (current) {
            case 'd': {
                const int32_t value = va_arg(args, int);

                const size_t written = write_dec(&buffer[position], (size - position), value);
                if (written == 0) goto cleanup;

                position += written;
                break;
            }
            case 'x': {
                const uint32_t value = va_arg(args, uint32_t);

                const size_t written = write_hex(&buffer[position], (size - position), value);
                if (written == 0) goto cleanup;

                position += written;
                break;
            }
            case 's': {
                const char *value = va_arg(args, char *);
                if (value == NULL) value = "(null)";

                const uint32_t length = strlen(value);
                if (!ENOUGH_SLOTS_FREE(length)) goto cleanup;

                const size_t written = write_buffer(&buffer[position], (size - position), value, length);
                if (written == 0) goto cleanup;

                position += written;
                break;
            }
            case 'c': {
                if (!ENOUGH_SLOTS_FREE(1)) goto cleanup;
                const char value = va_arg(args, int);
                buffer[position++] = value;
                break;
            }
            case '%': {
                if (!ENOUGH_SLOTS_FREE(1)) goto cleanup;
                buffer[position++] = '%';
                break;
            }
            default: {
                if (!ENOUGH_SLOTS_FREE(2)) goto cleanup;
                buffer[position++] = '%';
                buffer[position++] = current;
                break;
            }
        }
    }

cleanup:
    // This should never ever happen, but in the worst case scenario this won't lead to a buffer overflow.
    if (position >= size) position = size - 1;
    buffer[position] = '\0';

    return position;
}

uint32_t snprintf(char *buffer, const uint32_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);

    const uint32_t written = vsnprintf(buffer, size, format, args);

    va_end(args);
    return written;
}

uint32_t kprintf(const char *format, ...) {
    static const uint32_t PRINTF_BUFFER_SIZE = 256;
    char buffer[PRINTF_BUFFER_SIZE];

    va_list args;
    va_start(args, format);

    const uint32_t written = vsnprintf(buffer, PRINTF_BUFFER_SIZE, format, args);
    if (written != 0) vga_write(buffer);

    va_end(args);
    return written;
}
