#ifndef KMEMORY_H
#define KMEMORY_H

#include <stdint.h>

void* kmemcpy(void* destination, const void* source, uint32_t size);

void kmemset(void* destination, uint8_t value, uint32_t size);

#endif // KMEMORY_H
