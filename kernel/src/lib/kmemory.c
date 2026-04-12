#include "lib/kmemory.h"

#include <stddef.h>

void* kmemcpy(void* destination, const void* source, uint32_t size) {
    if (destination == NULL || source == NULL) {
        return NULL;
    }

    if (destination == source || size == 0) {
        return destination;
    }

    uint8_t* dest = (uint8_t*)destination;
    const uint8_t* src = (uint8_t*)source;

    while (size--) {
        *dest = *src;
        dest++;
        src++;
    }

    return destination;
}

void* kmemset(void* destination, uint8_t value, uint32_t size) {
    if (destination == NULL) {
        return NULL;
    }

    if (size == 0) {
        return destination;
    }

    uint8_t* dest = (uint8_t*)destination;

    while (size--) {
        *dest = value;
        dest++;
    }

    return destination;
}

uintptr_t kalign_up(const uintptr_t value, const uintptr_t alignment) {
    return (value + alignment - 1U) & ~(alignment - 1U);
}

uintptr_t kalign_down(const uintptr_t value, const uintptr_t alignment) {
    return value & ~(alignment - 1U);
}
