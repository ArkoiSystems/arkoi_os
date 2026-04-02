#include "lib/kmemory.h"
#include <stdint.h>

void *memcpy(void *destination, const void *source, uint32_t size) {
    uint8_t *dest = (uint8_t *) destination;
    const uint8_t *src = (uint8_t *) source;

    while (size--) {
        *dest = *src;
        dest++;
        src++;
    }

    return destination;
}
