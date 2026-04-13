#include "lib/kpanic.h"

#include "lib/kstdio.h"

void kpanic(const char* format, const char* file, const int line, const char* function, ...) {
    va_list args;
    va_start(args, function);

    kprintf("Panic occurred at \"%s:%u\" in \"%s\"!\n", file, line, function);
    kvprintf(format, args);
    kprintf("\n");

    va_end(args);

    // Panic must never return; disable interrupts and halt forever.
    __asm__ volatile("cli");
    for (;;) {
        __asm__ volatile("hlt");
    }
}
