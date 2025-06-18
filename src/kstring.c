#include "kstring.h"

char *memmove(char *destination, const char *source, const uint32_t size) {
    for (uint32_t index = 0; index < size; index++) {
        destination[index] = source[index];
    }
    return destination;
}

uint32_t strlen(const char *input) {
    uint32_t length = 0;
    while (input[length]) length++;
    return length;
}
