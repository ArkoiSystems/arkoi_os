#ifndef KPANIC_H
#define KPANIC_H

// A macro to simplify calling the kpanic function with file, line, and function information provided automatically.
#define KPANIC(format, ...) kpanic(format, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

/**
 * @brief Kernel panic function, prints the message and halts the system.
 *
 * This function is called when a critical error occurs in the kernel, and it will print the provided message along with
 * the source file, line number, and function name where the panic occurred. After printing the message, it will halt
 * the system to prevent further damage.
 *
 * @param format   The format string, similar to printf.
 * @param file     The source file where the panic occurred.
 * @param line     The line number in the source file where the panic occurred.
 * @param function The function name where the panic occurred.
 * @param ...      Additional arguments for the format string.
 */
void kpanic(const char* format, const char* file, int line, const char* function, ...);

#endif // KPANIC_H
