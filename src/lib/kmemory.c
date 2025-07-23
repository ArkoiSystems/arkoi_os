#include "lib/kmemory.h"

void *memcpy(void *destination, const void *source, uint32_t size) {
    char *dest = (char *) destination;
    const char *src = (char *) source;

    while (size--) {
        *dest = *src;
        dest++;
        src++;
    }

    return destination;
}
