#ifndef SERIAL_H
#define SERIAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "lib/kassert.h"

// Base clock frequency for the serial port, used to calculate the divisor for baud rate generation
#define SERIAL_BASE_CLOCK 115200

// Configuration for the first serial port (COM1)
#define SERIAL_PORT_COM1 0x3F8
#define SERIAL_IRQ_COM1 4

// Configuration for the second serial port (COM2)
#define SERIAL_PORT_COM2 0x2F8
#define SERIAL_IRQ_COM2 3

// Register offsets for the serial port
#define SERIAL_REG_DATA 0
#define SERIAL_REG_IER 1
#define SERIAL_REG_FCR 2
#define SERIAL_REG_LCR 3
#define SERIAL_REG_MCR 4
#define SERIAL_REG_LSR 5

#define SERIAL_REG_LSB 0
#define SERIAL_REG_MSB 1

// Line Control Register (LCR) helper masks and shifts
#define SERIAL_LCR_DATA_MASK 0b11
#define SERIAL_LCR_DATA_SHIFT 0
#define SERIAL_LCR_STOP_MASK 0b100
#define SERIAL_LCR_STOP_SHIFT 2
#define SERIAL_LCR_PARITY_MASK 0b1110000
#define SERIAL_LCR_PARITY_SHIFT 4

// Line Status Register (LSR) bits
#define SERIAL_LSR_DATA_READY 0x01
#define SERIAL_LSR_THR_EMPTY 0x20

// Helper macro for printing to a serial port
#define SERIAL_PRINT(port, text)                                                                                       \
    do {                                                                                                               \
        serial_port_t* serial_port = serial_get_port(port);                                                            \
        KASSERT(serial_write_string(serial_port, text));                                                               \
    } while (0);

/**
 * @brief Enumeration representing the parity mode used in serial communication.
 *
 * This enumeration defines the possible parity modes that can be used in serial communication. The values correspond to
 * the settings that can be applied to the Line Control Register (LCR) of a serial port, allowing for flexible
 * configuration of the parity mode used in serial communication. The default setting is no parity, which is commonly
 * used in many serial communication scenarios. The other options include odd parity, even parity, mark parity, and
 * space parity, providing additional error-checking capabilities for certain communication requirements.
 */
typedef enum {
    SERIAL_PARITY_NONE = 0b000,  /**< No parity */
    SERIAL_PARITY_ODD = 0b001,   /**< Odd parity */
    SERIAL_PARITY_EVEN = 0b011,  /**< Even parity */
    SERIAL_PARITY_MARK = 0b101,  /**< Mark parity */
    SERIAL_PARITY_SPACE = 0b111, /**< Space parity */
} serial_parity_t;

/**
 * @brief Enumeration representing the number of stop bits used in serial communication.
 *
 * This enumeration defines the possible configurations for the number of stop bits in a serial communication setup.
 * The values correspond to the settings that can be applied to the Line Control Register (LCR) of a serial port,
 * allowing for flexible configuration of the stop bits used in serial communication. The default setting is 1 stop
 * bit, which is commonly used in many serial communication scenarios. When used with 5 data bits, the 2 stop bits
 * setting can also represent 1.5 stop bits, providing additional flexibility for certain communication requirements.
 *
 */
typedef enum {
    SERIAL_STOP_BITS_1 = 0b0, /**< 1 stop bit */
    SERIAL_STOP_BITS_2 = 0b1, /**< 2 stop bits (the default) or 1.5 when used with 5 data bits */
} serial_stop_bits_t;

/**
 * @brief Enumeration representing the number of data bits used in serial communication.
 *
 * This enumeration defines the possible configurations for the number of data bits in a serial communication setup. The
 * values correspond to the settings that can be applied to the Line Control Register (LCR) of a serial port, allowing
 * for flexible configuration of the data bits used in serial communication. The default setting is 8 data bits, which
 * is commonly used in many serial communication scenarios.
 */
typedef enum {
    SERIAL_DATA_BITS_5 = 0b00, /**< 5 data bits */
    SERIAL_DATA_BITS_6 = 0b01, /**< 6 data bits */
    SERIAL_DATA_BITS_7 = 0b10, /**< 7 data bits */
    SERIAL_DATA_BITS_8 = 0b11, /**< 8 data bits (the default) */
} serial_data_bits_t;

/**
 * @brief Structure representing a serial port and its configuration.
 *
 * This structure holds the necessary information about a serial port, including its port number, baud rate, and a
 * pointer to the next serial port in a linked list. It is used to manage multiple serial ports and their configurations
 * within the system. Each initialized serial port is represented by an instance of this structure, allowing for easy
 * retrieval and management of serial ports throughout the system.
 */
typedef struct serial_port {
    uint16_t port;            /**< The port number of the serial port */
    uint32_t baudrate;        /**< The baud rate for the serial communication */
    struct serial_port* next; /**< Pointer to the next serial port in the linked list */
} serial_port_t;

/**
 * @brief Initializes the serial communication system.
 *
 * This function initializes the serial communication system by setting up the necessary hardware registers for the
 * configured serial ports. It typically initializes the standard COM1 and COM2 ports with default settings, allowing
 * for immediate use of the serial ports for communication. The function may also perform tests to ensure that the
 * serial ports are functioning correctly after initialization. Once initialized, the serial ports can be accessed and
 * used for sending and receiving data through the defined API functions.
 */
void serial_init();

/**
 * @brief Initializes a serial port with the given parameters.
 *
 * This function sets up the specified serial port with the provided baud rate, data bits, parity, and stop bits. It
 * configures the necessary hardware registers to enable communication through the serial port. The initialized port is
 * added to a linked list of serial ports for later retrieval.
 *
 * @param port      The port number of the serial port to initialize (e.g., 0x3F8 for COM1).
 * @param baudrate  The baud rate for the serial communication (e.g., 9600, 115200).
 * @param data_bits The number of data bits to use in the serial communication (e.g., 5, 6, 7, or 8).
 * @param parity    The parity mode to use for the serial communication (e.g., none, odd, even, mark, or space).
 * @param stop_bits The number of stop bits to use in the serial communication (e.g., 1 or 2).
 * @return serial_port_t* Returns a pointer to the `serial_port_t` structure representing the initialized serial port,
 *                        or NULL if the initialization failed.
 */
serial_port_t* serial_init_port(
    uint16_t port, uint32_t baudrate, serial_data_bits_t data_bits, serial_parity_t parity,
    serial_stop_bits_t stop_bits);

/**
 * @brief Retrieves a pointer to the `serial_port_t` structure corresponding to the specified port number.
 *
 * This function searches through the linked list of initialized serial ports to find the one that matches the given
 * port number. If a matching port is found, a pointer to its `serial_port_t` structure is returned.
 *
 * @param port The port number of the serial port to retrieve (e.g., 0x3F8 for COM1).
 * @return serial_port_t* Returns a pointer to the `serial_port_t` structure representing the specified serial port, or
 *                        NULL if no such port has been initialized.
 */
serial_port_t* serial_get_port(uint16_t port);

/**
 * @brief Reads a single character from the specified serial port and stores it in the provided character pointer.
 *
 * This function waits until there is data available in the receive buffer of the specified serial port, then reads a
 * single character from the data register and stores it in the provided character pointer.
 *
 * @param serial_port A pointer to the `serial_port_t` structure representing the serial port to read from.
 * @param character   A pointer to a character variable where the read character will be stored.
 * @return bool Returns true if a character was successfully read from the serial port, or false if an error occurred
 *              during reading.
 */
bool serial_read_char(const serial_port_t* serial_port, char* character);

/**
 * @brief Reads a string from the specified serial port into the provided buffer.
 *
 * This function reads characters from the specified serial port and stores them in the provided buffer until a newline
 * character is encountered or the buffer size limit is reached. The function ensures that the buffer is null-terminated
 * after reading the string.
 *
 * @param serial_port A pointer to the `serial_port_t` structure representing the serial port to read from.
 * @param buffer      A pointer to the buffer where the read string will be stored.
 * @param buffer_size The size of the buffer, including space for the null terminator.
 * @return bool Returns true if a string was successfully read from the serial port, or false if an error occurred
 *              during reading.
 */
bool serial_read_string(const serial_port_t* serial_port, char* buffer, size_t buffer_size);

/**
 * @brief Writes a single character to the specified serial port.
 *
 * This function sends a single character to the specified serial port. It waits until the transmit holding register is
 * empty before sending the character, ensuring that the character is transmitted properly.
 *
 * @param serial_port A pointer to the serial port structure representing the target serial port.
 * @param character   The character to be sent to the serial port.
 * @return bool Returns true if the character was successfully sent, or false if an error occurred during transmission.
 */
bool serial_write_char(const serial_port_t* serial_port, char character);

/**
 * @brief Writes a null-terminated string to the specified serial port.
 *
 * This function sends each character of the provided null-terminated string to the specified serial port.
 * It ensures that the transmit holding register is empty before sending each character, allowing for proper
 * transmission.
 *
 * @param serial_port A pointer to the serial port structure representing the target serial port.
 * @param text        A pointer to the null-terminated string to be sent to the serial port.
 * @return bool Returns true if the string was successfully sent, or false if an error occurred during transmission.
 */
bool serial_write_string(const serial_port_t* serial_port, const char* text);

#endif // SERIAL_H
