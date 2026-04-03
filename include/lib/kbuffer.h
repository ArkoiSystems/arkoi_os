#ifndef KBUFFER_H
#define KBUFFER_H

#include <stdbool.h>
#include <stddef.h>

typedef struct cyclic_buffer {
    void* buffer;        // Pointer to the buffer memory
    size_t head;         // Index of the head of the buffer
    size_t tail;         // Index of the tail of the buffer
    size_t capacity;     // Total capacity of the buffer
    size_t element_size; // Size of each buffered element
} cyclic_buffer_t;

void cyclic_buffer_init(cyclic_buffer_t* cb, void* buffer, size_t capacity, size_t element_size);

void cyclic_buffer_push(cyclic_buffer_t* cb, void* source);

void cyclic_buffer_pop(cyclic_buffer_t* cb, void* destination);

size_t cyclic_buffer_size(cyclic_buffer_t* cb);

bool cyclic_buffer_is_full(cyclic_buffer_t* cb);

bool cyclic_buffer_is_empty(cyclic_buffer_t* cb);

#endif // KBUFFER_H
