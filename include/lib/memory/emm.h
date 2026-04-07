#ifndef EMM_H
#define EMM_H

#include <stddef.h>

/**
 * @brief Allocates memory of the given size.
 *
 * This function provides the ability to allocate memory dynamically early in the system's initialization process,
 * before the standard memory management facilities are fully set up. It is typically used for allocating memory for
 * critical data structures that need to be available during the early stages of system boot.
 *
 * @param size The size of the memory to allocate, in bytes.
 * @return void* A pointer to the allocated memory, or NULL if the allocation fails.
 */
void* emm_alloc(size_t size);

#endif // EMM_H
