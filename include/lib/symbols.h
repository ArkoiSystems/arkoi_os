#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdint.h>

#define SYMBOL_START(name) ((uint32_t) &_##name##_start)
#define SYMBOL_END(name)   ((uint32_t) &_##name##_end)
#define SYMBOL_SIZE(name)  (SYMBOL_END(name) - SYMBOL_START(name))

#define SYMBOL_DECLARE(name)    \
    extern void *_##name##_start; \
    extern void *_##name##_end;

SYMBOL_DECLARE(text)
SYMBOL_DECLARE(rodata)
SYMBOL_DECLARE(data)
SYMBOL_DECLARE(bss)

#undef SYMBOL_DECLARE

#endif //SYMBOLS_H
