#include "lib/kmemory.h"

void *memcpy(void *destination, const void *source, uint32_t size) {
    while (size--) {
        *((char *) destination) = *((char *) source);
        destination++;
        source++;
    }
    return destination;
}
