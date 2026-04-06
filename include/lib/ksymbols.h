#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdint.h>

#define SYMBOL_START(name) ((uintptr_t) & _##name##_start)
#define SYMBOL_END(name) ((uintptr_t) & _##name##_end)
#define SYMBOL_SIZE(name) (SYMBOL_END(name) - SYMBOL_START(name))

extern uintptr_t _kernel_physical_base;
extern uintptr_t _kernel_virtual_base;

#define KERNEL_PHYSICAL_BASE ((uintptr_t) & _kernel_physical_base)
#define KERNEL_VIRTUAL_BASE ((uintptr_t) & _kernel_virtual_base)

#define SYMBOL_DECLARE(name)                                                                                           \
    extern void* _##name##_start;                                                                                      \
    extern void* _##name##_end;

SYMBOL_DECLARE(kernel)
SYMBOL_DECLARE(text)
SYMBOL_DECLARE(rodata)
SYMBOL_DECLARE(data)
SYMBOL_DECLARE(bss)
SYMBOL_DECLARE(early_heap)
SYMBOL_DECLARE(heap)

#undef SYMBOL_DECLARE

#endif // SYMBOLS_H
