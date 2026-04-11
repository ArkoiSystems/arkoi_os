#ifndef KIO_H
#define KIO_H

#include <stdint.h>

/**
 * @brief Write a byte to the specified port.
 *
 * This function writes a byte of data to the specified I/O port. It is commonly used in low-level programming to
 * interact with hardware devices.
 *
 * @param port The I/O port to write to.
 * @param data The byte of data to write to the port.
 */
void outb(uint16_t port, uint8_t data);

/**
 * @brief Read a byte from the specified port.
 *
 * This function reads a byte of data from the specified I/O port. It is commonly used in low-level programming to
 * interact with hardware devices.
 *
 * @param port The I/O port to read from.
 * @return uint8_t The byte of data read from the port.
 */
uint8_t inb(uint16_t port);

#endif // KIO_H
