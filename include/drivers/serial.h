#ifndef SERIAL_H
#define SERIAL_H

#include <stdbool.h>
#include <stdint.h>

#include "lib/kbuffer.h"

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

typedef enum { SERIAL_BAUD_9600 = 12, SERIAL_BAUD_38400 = 3, SERIAL_BAUD_115200 = 1 } serial_baud_t;

bool serial_init_port(uint16_t port, serial_baud_t baud);

bool serial_get_char(uint16_t port, char* character);

bool serial_write_char(uint16_t port, char character);

bool serial_write_string(uint16_t port, const char* text);

#endif // SERIAL_H
