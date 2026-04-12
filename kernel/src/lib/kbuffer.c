#include "lib/kbuffer.h"

#include "lib/kmemory.h"

bool cyclic_buffer_init(cyclic_buffer_t* cb, void* buffer, size_t capacity, size_t element_size) {
    if (cb == NULL || buffer == NULL || capacity == 0 || element_size == 0) {
        return false;
    }

    cb->buffer = buffer;
    cb->head = 0;
    cb->tail = 0;
    cb->capacity = capacity;
    cb->element_size = element_size;

    return true;
}

bool cyclic_buffer_push(cyclic_buffer_t* cb, void* source) {
    if (cb == NULL || source == NULL) {
        return false;
    }

    bool was_full = cyclic_buffer_is_full(cb);

    void* destination = (uint8_t*)cb->buffer + (cb->head * cb->element_size);
    kmemcpy(destination, source, cb->element_size);

    cb->head = (cb->head + 1) % cb->capacity;

    if (was_full) {
        cb->tail = (cb->tail + 1) % cb->capacity;
    }

    return true;
}

bool cyclic_buffer_pop(cyclic_buffer_t* cb, void* destination) {
    if (cb == NULL || destination == NULL) {
        return false;
    }

    if (cyclic_buffer_is_empty(cb)) {
        return false;
    }

    void* source = (uint8_t*)cb->buffer + (cb->tail * cb->element_size);
    kmemcpy(destination, source, cb->element_size);

    cb->tail = (cb->tail + 1) % cb->capacity;
    return true;
}

size_t cyclic_buffer_size(cyclic_buffer_t* cb) {
    if (cb == NULL) {
        return 0;
    }

    return ((cb->head + cb->capacity) - cb->tail) % cb->capacity;
}

bool cyclic_buffer_is_full(cyclic_buffer_t* cb) {
    if (cb == NULL) {
        return false;
    }

    return ((cb->head + 1) % cb->capacity) == cb->tail;
}

bool cyclic_buffer_is_empty(cyclic_buffer_t* cb) {
    if (cb == NULL) {
        return false;
    }

    return (cb->head == cb->tail);
}
