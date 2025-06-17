#ifndef KSTDIO_H
#define KSTDIO_H

/**
 * Prints a formatted string to the VGA text mode terminal.
 *
 * Supports the following format specifiers:
 * - %d: Signed decimal integer
 * - %x: Unsigned hexadecimal integer
 * - %s: Null-terminated string
 * - %c: Single character
 * - %%: Literal percent sign
 *
 * Any invalid format specifier will be printed as is, prefixed by '%'.
 *
 * @param message The format string containing text and format specifiers.
 * @param ... Additional arguments corresponding to the format specifiers in the format string.
 */
void kprintf(const char *message, ...);

#endif //KSTDIO_H
