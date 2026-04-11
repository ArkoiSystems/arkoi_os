#ifndef PIC_H
#define PIC_H

#include <stdint.h>

// Definitions for the first (master) PIC
#define PIC1 0x20
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)

// Definitions for the second (slave) PIC
#define PIC2 0xA0
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define ICW1_ICW4 0x01      // Indicates that ICW4 will be present
#define ICW1_SINGLE 0x02    // Single (cascade) mode
#define ICW1_INTERVAL4 0x04 // Call address interval 4 (8)
#define ICW1_LEVEL 0x08     // Level triggered (edge) mode
#define ICW1_INIT 0x10      // Initialization - required!

#define ICW4_8086 0x01       // 8086/88 (MCS-80/85) mode
#define ICW4_AUTO 0x02       // Auto (normal) EOI
#define ICW4_BUF_SLAVE 0x08  // Buffered mode/slave
#define ICW4_BUF_MASTER 0x0C // Buffered mode/master
#define ICW4_SFNM 0x10       // Special fully nested (not)

#define CASCADE_IRQ 2

#define PIC_EOI 0x20

/**
 * @brief Remaps the PIC to new vector offsets.
 *
 * This function initializes the PICs and remaps their interrupt vector offsets to the specified values. The master
 * PIC will be remapped to start at `offset_1`, and the slave PIC will be remapped to start at `offset_2`. This is
 * typically done to avoid conflicts with CPU exceptions and to organize the interrupt vector table more effectively.
 *
 * @param offset_1
 * @param offset_2
 */
void pic_remap(uint8_t offset_1, uint8_t offset_2);

/**
 * @brief Sends an End of Interrupt (EOI) signal to the PIC for a specific IRQ.
 *
 * This function is used to notify the PIC that the CPU has finished handling an interrupt associated with the *
 * specified IRQ.
 *
 * @param irq The IRQ number for which to send EOI (0-15).
 */
void pic_send_eoi(uint8_t irq);

/**
 * @brief Sets the mask for a specific IRQ, preventing it from being sent to the CPU.
 *
 * This function takes an IRQ number as input and sets the corresponding bit in the PIC's interrupt mask register,
 * effectively masking the specified IRQ and preventing it from being sent to the CPU when it occurs. This is typically
 * used to temporarily disable specific hardware interrupts while allowing others to be handled.
 *
 * @param irq The IRQ number for which to set the mask (0-15).
 * @return uint8_t The previous value of the interrupt mask register before setting the specified IRQ's bit.
 */
uint8_t pic_set_mask(uint8_t irq);

/**
 * @brief Clears the mask for a specific IRQ, allowing it to be sent to the CPU.
 *
 * This function takes an IRQ number as input and clears the corresponding bit in the PIC's interrupt mask register,
 * enabling the specified IRQ to be sent to the CPU when it occurs. This is typically used to allow specific hardware
 * interrupts to be handled by the CPU while keeping others masked.
 *
 * @param irq The IRQ number for which to clear the mask (0-15).
 * @return uint8_t The previous value of the interrupt mask register before clearing the specified IRQ's bit.
 */
uint8_t pic_clear_mask(uint8_t irq);

/**
 * @brief Disables the PIC by masking all interrupts.
 *
 * This function effectively disables the PIC by setting the interrupt mask to 0xFF for both the master and slave PICs,
 * preventing any interrupts from being sent to the CPU. This is typically done when transitioning to a more advanced
 * interrupt controller like APIC or when the PIC is no longer needed.
 *
 */
void pic_disable();

#endif // PIC_H
