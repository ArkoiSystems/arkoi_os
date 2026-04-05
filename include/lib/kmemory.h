#ifndef KMEMORY_H
#define KMEMORY_H

#include <stdint.h>

void* kmemcpy(void* destination, const void* source, uint32_t size);

void kmemset(void* destination, uint8_t value, uint32_t size);

uintptr_t kalign_up(const uintptr_t value, const uintptr_t alignment);

uintptr_t kalign_down(const uintptr_t value, const uintptr_t alignment);

#endif // KMEMORY_H
