#include "kpanic.h"

#include "kstdio.h"

void panic(const char *format, const char *file, const int line, const char *function, ...) {
    va_list args;
    va_start(args, format);

    kprintf("Panic occurred at \"%s:%d\" in \"%s\"!\n", file, line, function);
    kvprintf(format, args);

    va_end(args);
}
