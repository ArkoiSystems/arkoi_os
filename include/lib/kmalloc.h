#ifndef KMALLOC_H
#define KMALLOC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_ORDER (20U)  // 2^20 = 1048576 bytes (1 MiB)
#define MIN_ORDER (6U)   // 2^6 = 64 bytes

typedef struct block {
    struct block *next;
    uint8_t order;
    bool is_free;
} block_t;

void kmalloc_init();

void *kmalloc(size_t size);

void kfree(void *ptr);

#endif //KMALLOC_H
