#ifndef KBUFFER_H
#define KBUFFER_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Structure representing a cyclic buffer (also known as a ring buffer).
 *
 * The cyclic buffer is a fixed-size data structure that uses a single, contiguous block of memory to store elements. It
 * operates in a circular manner, where the head and tail indices wrap around when they reach the end of the buffer.
 * This allows for efficient use of memory and fast insertion and removal of elements without the need for shifting
 * data.
 */
typedef struct cyclic_buffer {
    void* buffer;        /**< Pointer to the buffer memory */
    size_t head;         /**< Index of the head of the buffer */
    size_t tail;         /**< Index of the tail of the buffer */
    size_t capacity;     /**< Total capacity of the buffer */
    size_t element_size; /**< Size of each buffered element */
} cyclic_buffer_t;

/**
 * @brief Initializes a cyclic buffer with the provided parameters.
 *
 * This function sets up the cyclic buffer structure with the given buffer memory, capacity, and element size. The head
 * and tail indices are initialized to zero.
 *
 * @param cb           Pointer to the cyclic buffer structure to initialize
 * @param buffer       Pointer to the pre-allocated buffer memory that will be used for storing elements
 * @param capacity     Total number of elements that the buffer can hold
 * @param element_size Size of each element in bytes
 */
void cyclic_buffer_init(cyclic_buffer_t* cb, void* buffer, size_t capacity, size_t element_size);

/**
 * @brief Pushes an element into the cyclic buffer.
 *
 * This function adds an element to the cyclic buffer, if there is space available. The element is copied from the
 * source pointer.
 *
 * @param cb     Pointer to the cyclic buffer structure
 * @param source Pointer to the element to be pushed into the buffer
 */
void cyclic_buffer_push(cyclic_buffer_t* cb, void* source);

/**
 * @brief Pops an element from the cyclic buffer.
 *
 * This function removes an element from the cyclic buffer and copies it to the destination pointer. The head index
 * is updated accordingly.
 *
 * @param cb          Pointer to the cyclic buffer structure
 * @param destination Pointer to the memory location where the popped element will be copied
 */
void cyclic_buffer_pop(cyclic_buffer_t* cb, void* destination);

/**
 * @brief Returns the current number of elements in the cyclic buffer.
 *
 * This function calculates the number of elements currently stored in the cyclic buffer by comparing the head and tail
 * indices.
 *
 * @param cb      Pointer to the cyclic buffer structure
 * @return size_t The number of elements currently in the buffer
 */
size_t cyclic_buffer_size(cyclic_buffer_t* cb);

/**
 * @brief Checks if the cyclic buffer is full.
 *
 * This function determines if the cyclic buffer has reached its maximum capacity by comparing the head and tail
 * indices.
 *
 * @param cb Pointer to the cyclic buffer structure
 * @return bool True if the buffer is full, false otherwise
 */
bool cyclic_buffer_is_full(cyclic_buffer_t* cb);

/**
 * @brief Checks if the cyclic buffer is empty.
 *
 * This function checks if the cyclic buffer contains no elements by comparing the head and tail indices.
 *
 * @param cb Pointer to the cyclic buffer structure
 * @return bool True if the buffer is empty, false otherwise
 */
bool cyclic_buffer_is_empty(cyclic_buffer_t* cb);

#endif // KBUFFER_H
