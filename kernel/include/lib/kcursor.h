#ifndef KCURSOR_H
#define KCURSOR_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief A structure representing a cursor for managing a buffer of data.
 */
typedef struct kcursor {
    uint8_t* start;   /**< The start of the buffer. */
    uint8_t* current; /**< The current position in the buffer. */
    uint8_t* end;     /**< The end of the buffer. */
} kcursor_t;

/**
 * @brief Initializes a cursor with the given parameters.
 *
 * This function sets up the cursor's start, current, and end pointers based on the provided buffer and size. The start
 * pointer is set to the beginning of the buffer, the current pointer is also initialized to the start, and the end
 * pointer is set to the start plus the size of the buffer. This allows the cursor to track its position within the
 * buffer and ensure that it does not exceed the allocated space when writing data.
 *
 * @param cursor The cursor to initialize.
 * @param buffer The buffer that the cursor will manage.
 * @param size   The size of the buffer in bytes.
 * @return bool True if the cursor was successfully initialized, false if the buffer is null or the size is zero.
 */
bool kcursor_init(kcursor_t* cursor, void* buffer, uint32_t size);

/**
 * @brief Calculates the number of bytes remaining in the cursor before reaching the end.
 *
 * @param cursor The cursor to calculate the remaining bytes for.
 * @return uint32_t The number of bytes remaining in the cursor before reaching the end.
 */
uint32_t kcursor_remaining(const kcursor_t* cursor);

/**
 * @brief Calculates the number of bytes that have been written to the cursor since it was initialized or last reset.
 *
 * @param cursor The cursor to calculate the written bytes for.
 * @return uint32_t The number of bytes that have been written to the cursor.
 */
uint32_t kcursor_written(const kcursor_t* cursor);

/**
 * @brief Advances the cursor by a specified amount of bytes.
 *
 * @param cursor The cursor to advance.
 * @param amount The number of bytes to advance the cursor by.
 * @return bool True if the cursor was successfully advanced, false if advancing would exceed the cursor's end.
 */
bool kcursor_advance(kcursor_t* cursor, uint32_t amount);

/**
 * @brief Writes a single byte to the cursor, advancing it by one byte.
 *
 * This function checks if there is enough space in the cursor to write a single byte. If there is enough space, it
 * writes the byte to the cursor's current position and advances the cursor by one byte. If there is not enough space,
 * it returns false and does not modify the cursor.
 *
 * @param cursor The cursor to write to.
 * @param value  The byte value to write.
 * @return bool True if the byte was successfully written, false if there was not enough space in the cursor.
 */
bool kcursor_write_byte(kcursor_t* cursor, uint8_t value);

/**
 * @brief Writes a block of data to the cursor, advancing it by the length of the data.
 *
 * This function checks if there is enough space in the cursor to write the specified length of data. If there is enough
 * space, it copies the data from the source to the cursor's current position and advances the cursor by the length of
 * the data. If there is not enough space, it returns false and does not modify the cursor.
 *
 * @param cursor The cursor to write to.
 * @param source The source data to write from.
 * @param length The length of the data to write.
 * @return bool True if the data was successfully written, false if there was not enough space in the cursor.
 */
bool kcursor_write(kcursor_t* cursor, const void* source, uint32_t length);

#endif // KCURSOR_H
