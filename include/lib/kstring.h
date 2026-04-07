#ifndef KSTRING_HPP
#define KSTRING_HPP

#include <stdint.h>

/**
 * @brief Get the length of a string.
 *
 * This function calculates the length of a null-terminated string by iterating through each character until it
 * encounters the null terminator ('\0'). It returns the number of characters in the string, excluding the null
 * terminator.
 *
 * @param input A pointer to the null-terminated string whose length is to be calculated.
 * @return uint32_t The length of the string, excluding the null terminator.
 */
uint32_t kstrlen(const char* input);

#endif // KSTRING_HPP
