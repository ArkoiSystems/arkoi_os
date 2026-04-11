#ifndef KMEMORY_H
#define KMEMORY_H

#include <stdint.h>

/**
 * @brief Copies a block of memory from a source to a destination.
 *
 * This function copies `size` bytes from the memory area pointed to by `source` to the memory area pointed to by
 * `destination`. The memory areas should not overlap; if they do, the behavior is undefined. The function returns a
 * pointer to the destination, which is the same as the `destination` parameter. This function is typically used for
 * copying raw memory, such as when working with buffers or structures in low-level programming contexts.
 *
 * @param destination The pointer to the block of memory where the content will be copied to.
 * @param source      The pointer to the block of memory to copy from.
 * @param size        The number of bytes to copy.
 * @return void* A pointer to the destination, which is the same as the `destination` parameter.
 */
void* kmemcpy(void* destination, const void* source, uint32_t size);

/**
 * @brief Sets a block of memory to a specified value.
 *
 * This function fills the first `size` bytes of the memory area pointed to by `destination` with the constant byte
 * value `value`. This is typically used to initialize or reset memory to a known state. For example, setting memory to
 * zero can be useful for clearing buffers or initializing structures. The function does not return a value, as it
 * directly modifies the memory pointed to by `destination`.
 *
 * @param destination The pointer to the block of memory to fill.
 * @param value       The value to set.
 * @param size        The number of bytes to fill.
 */
void kmemset(void* destination, uint8_t value, uint32_t size);

/**
 * @brief Aligns the given value up to the nearest multiple of the specified alignment.
 *
 * This function takes a value and an alignment, and returns the smallest value that is greater than or equal to the
 * input value and is a multiple of the alignment. This is useful for ensuring that memory addresses or sizes are
 * properly aligned according to the specified requirements.
 *
 * @param value     The value to be aligned up.
 * @param alignment The alignment to which the value should be aligned up. This should be a power of two for proper
 *                  alignment.
 * @return uintptr_t The aligned value, which is the smallest multiple of the alignment that is greater than or equal to
 *                   the input value.
 */
uintptr_t kalign_up(const uintptr_t value, const uintptr_t alignment);

/**
 * @brief Aligns the given value down to the nearest multiple of the specified alignment.
 *
 * This function takes a value and an alignment, and returns the largest value that is less than or equal to the input
 * value and is a multiple of the alignment. This is useful for ensuring that memory addresses or sizes are properly
 * aligned according to the specified requirements.
 *
 * @param value     The value to be aligned down.
 * @param alignment The alignment to which the value should be aligned down. This should be a power of two for proper
 *                  alignment.
 * @return uintptr_t The aligned value, which is the largest multiple of the alignment that is less than or equal to the
 *                   input value.
 */
uintptr_t kalign_down(const uintptr_t value, const uintptr_t alignment);

#endif // KMEMORY_H
