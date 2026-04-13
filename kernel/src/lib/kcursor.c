#include "lib/kcursor.h"

#include <stddef.h>

#include "lib/kmemory.h"

bool kcursor_init(kcursor_t* cursor, void* buffer, uint32_t size) {
    if (cursor == NULL || buffer == NULL || size == 0) {
        return false;
    }

    cursor->start = (uint8_t*)buffer;
    cursor->current = cursor->start;
    cursor->end = cursor->start + size;

    return true;
}

uint32_t kcursor_remaining(const kcursor_t* cursor) {
    if (cursor == NULL) {
        return 0;
    }

    return (uint32_t)(cursor->end - cursor->current);
}

uint32_t kcursor_written(const kcursor_t* cursor) {
    if (cursor == NULL) {
        return 0;
    }

    return (uint32_t)(cursor->current - cursor->start);
}

bool kcursor_advance(kcursor_t* cursor, uint32_t amount) {
    if (cursor == NULL) {
        return false;
    }

    if (amount > kcursor_remaining(cursor)) {
        return false;
    }

    cursor->current += amount;
    return true;
}

bool kcursor_write_byte(kcursor_t* cursor, uint8_t value) {
    if (cursor == NULL) {
        return false;
    }

    if (kcursor_remaining(cursor) == 0) {
        return false;
    }

    *cursor->current = value;
    cursor->current++;
    return true;
}

bool kcursor_write(kcursor_t* cursor, const void* source, uint32_t length) {
    if (cursor == NULL || source == NULL) {
        return false;
    }

    if (length == 0) {
        return true;
    }

    if (length > kcursor_remaining(cursor)) {
        return false;
    }

    kmemcpy(cursor->current, source, length);
    cursor->current += length;
    return true;
}
