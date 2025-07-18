#include "lib/kstring.h"

uint32_t strlen(const char *input) {
    uint32_t length = 0;
    while (input[length]) length++;
    return length;
}
