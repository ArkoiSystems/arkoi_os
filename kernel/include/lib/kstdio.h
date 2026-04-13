#ifndef KSTDIO_H
#define KSTDIO_H

#include <stdarg.h>
#include <stdint.h>

/**
 * @brief Print formatted output to a string buffer using a va_list of arguments, similar to vsnprintf in C.
 *
 * This function takes a format string and a va_list of arguments, formats the output according to the format string,
 * and stores it in the provided buffer.
 *
 * Supported specifiers: %d, %i, %u, %x, %X, %p, %s, %c, and %%.
 *
 * @param buffer The buffer where the formatted string will be stored.
 * @param size   The maximum number of characters to be written to the buffer, including the null terminator.
 * @param format The format string, similar to printf in C.
 * @param args   A va_list of arguments to be formatted according to the format string.
 * @return uint32_t The number of characters written to the buffer, excluding the null terminator.
 */
uint32_t kvsnprintf(char* buffer, uint32_t size, const char* format, va_list args);

/**
 * @brief Print formatted output to a string buffer, similar to snprintf in C.
 *
 * This function takes a format string and a variable number of arguments, formats the output according to the format
 * string, and stores it in the provided buffer.
 *
 * Supported specifiers: %d, %i, %u, %x, %X, %p, %s, %c, and %%.
 *
 * @param buffer The buffer where the formatted string will be stored.
 * @param size   The maximum number of characters to be written to the buffer, including the null terminator.
 * @param format The format string, similar to printf in C.
 * @param ...    Additional arguments to be formatted according to the format string.
 * @return uint32_t The number of characters written to the buffer, excluding the null terminator.
 */
uint32_t ksnprintf(char* buffer, uint32_t size, const char* format, ...);

/**
 * @brief Print formatted output to the console using a va_list of arguments.
 *
 * This function takes a format string and a va_list of arguments, formats the output according to the format string,
 * and prints it to the console.
 *
 * Supported specifiers: %d, %i, %u, %x, %X, %p, %s, %c, and %%.
 *
 * @param format The format string, similar to printf in C.
 * @param args   A va_list of arguments to be formatted according to the format string.
 * @return uint32_t The number of characters printed to the console, excluding the null terminator.
 */
uint32_t kvprintf(const char* format, va_list args);

/**
 * @brief Print formatted output to the console.
 *
 * This function takes a format string and a variable number of arguments, formats the output according to the format
 * string, and prints it to the console.
 *
 * Supported specifiers: %d, %i, %u, %x, %X, %p, %s, %c, and %%.
 *
 * @param format The format string, similar to printf in C.
 * @param ...    Additional arguments to be formatted according to the format string.
 * @return uint32_t The number of characters printed to the console, excluding the null terminator.
 */
uint32_t kprintf(const char* format, ...);

#endif // KSTDIO_H
