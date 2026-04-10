#ifndef SERIAL_H
#define SERIAL_H

#include <stdbool.h>
#include <stdint.h>

#include "lib/kbuffer.h"

#define SERIAL_BASE_CLOCK 115200

#define SERIAL_PORT_COM1 0x3F8
#define SERIAL_PORT_COM2 0x2F8

#define SERIAL_IRQ_COM1 4
#define SERIAL_IRQ_COM2 3

#define SERIAL_REG_DATA 0
#define SERIAL_REG_IER 1
#define SERIAL_REG_LCR 3
#define SERIAL_REG_LSR 5
#define SERIAL_REG_FCR 2
#define SERIAL_REG_MCR 4
#define SERIAL_REG_LSB 0
#define SERIAL_REG_MSB 1

#define SERIAL_LCR_DATA_MASK 0b11
#define SERIAL_LCR_DATA_SHIFT 0
#define SERIAL_LCR_STOP_MASK 0b100
#define SERIAL_LCR_STOP_SHIFT 2
#define SERIAL_LCR_PARITY_MASK 0b1110000
#define SERIAL_LCR_PARITY_SHIFT 4

#define SERIAL_LSR_DATA_READY 0x01
#define SERIAL_LSR_THR_EMPTY 0x20

typedef enum {
    SERIAL_PARITY_NONE = 0b000,  /**< No parity */
    SERIAL_PARITY_ODD = 0b001,   /**< Odd parity */
    SERIAL_PARITY_EVEN = 0b011,  /**< Even parity */
    SERIAL_PARITY_MARK = 0b101,  /**< Mark parity */
    SERIAL_PARITY_SPACE = 0b111, /**< Space parity */
} serial_parity_t;

typedef enum {
    SERIAL_STOP_BITS_1 = 0b0, /**< 1 stop bit */
    SERIAL_STOP_BITS_2 = 0b1, /**< 2 stop bits (the default) or 1.5 when used with 5 data bits */
} serial_stop_bits_t;

typedef enum {
    SERIAL_DATA_BITS_5 = 0b00, /**< 5 data bits */
    SERIAL_DATA_BITS_6 = 0b01, /**< 6 data bits */
    SERIAL_DATA_BITS_7 = 0b10, /**< 7 data bits */
    SERIAL_DATA_BITS_8 = 0b11, /**< 8 data bits (the default) */
} serial_data_bits_t;

typedef struct serial_port_config {
    uint16_t port;
    uint32_t baudrate;
    struct serial_port_config* next;
} serial_port_config_t;

bool serial_init_port(
    uint16_t port, uint32_t baudrate, serial_data_bits_t data_bits, serial_parity_t parity,
    serial_stop_bits_t stop_bits);

bool serial_read_char(uint16_t port, char* character);

bool serial_read_string(uint16_t port, char* buffer, size_t buffer_size);

bool serial_write_char(uint16_t port, char character);

bool serial_write_string(uint16_t port, const char* text);

#endif // SERIAL_H
