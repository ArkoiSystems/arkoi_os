#include "lib/kbuffer.h"

#include "lib/kmemory.h"

void cyclic_buffer_init(cyclic_buffer_t* cb, void* buffer, size_t capacity, size_t element_size) {
    if (cb == NULL || buffer == NULL || capacity == 0 || element_size == 0) {
        // TODO(tbd): Report error
        return;
    }

    cb->buffer = buffer;
    cb->head = 0;
    cb->tail = 0;
    cb->capacity = capacity;
    cb->element_size = element_size;
}

void cyclic_buffer_push(cyclic_buffer_t* cb, void* source) {
    if (cb == NULL || source == NULL) {
        // TODO(tbd): Report error
        return;
    }

    if (cyclic_buffer_is_full(cb)) {
        // TODO(tbd): Report error
        return;
    }

    void* destination = (uint8_t*)cb->buffer + (cb->head * cb->element_size);
    memcpy(destination, source, cb->element_size);

    cb->head = (cb->head + 1) % cb->capacity;
}

void cyclic_buffer_pop(cyclic_buffer_t* cb, void* destination) {
    if (cb == NULL || destination == NULL) {
        // TODO(tbd): Report error
        return;
    }

    if (cyclic_buffer_is_empty(cb)) {
        // TODO(tbd): Report error
        return;
    }

    void* source = (uint8_t*)cb->buffer + (cb->tail * cb->element_size);
    memcpy(destination, source, cb->element_size);

    cb->tail = (cb->tail + 1) % cb->capacity;
}

size_t cyclic_buffer_size(cyclic_buffer_t* cb) {
    if (cb == NULL) {
        // TODO(tbd): Report error
        return 0;
    }

    return ((cb->head + cb->capacity) - cb->tail) % cb->capacity;
}

bool cyclic_buffer_is_full(cyclic_buffer_t* cb) {
    if (cb == NULL) {
        // TODO(tbd): Report error
        return false;
    }

    return ((cb->head + 1) % cb->capacity) == cb->tail;
}

bool cyclic_buffer_is_empty(cyclic_buffer_t* cb) {
    if (cb == NULL) {
        // TODO(tbd): Report error
        return false;
    }

    return (cb->head == cb->tail);
}
