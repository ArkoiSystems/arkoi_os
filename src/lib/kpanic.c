#include "lib/kpanic.h"

#include "lib/kstdio.h"

void panic(const char *format, const char *file, const int line, const char *function, ...) {
    va_list args;
    va_start(args, function);

    kprintf("Panic occurred at \"%s:%d\" in \"%s\"!\n", file, line, function);
    kvprintf(format, args);
    kprintf("\n");

    va_end(args);
}
