#ifndef KSTDIO_H
#define KSTDIO_H

#include <stdarg.h>
#include <stdint.h>

uint32_t vsnprintf(char *buffer, uint32_t size, const char *format, va_list args);

uint32_t snprintf(char *buffer, uint32_t size, const char *format, ...);

uint32_t kprintf(const char *format, ...);

#endif //KSTDIO_H
