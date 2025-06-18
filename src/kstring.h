#ifndef KSTRING_HPP
#define KSTRING_HPP

#include <stdint.h>

char *memmove(char *destination, const char *source, uint32_t size);

uint32_t strlen(const char *input);

#endif //KSTRING_HPP
