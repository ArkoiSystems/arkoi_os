#ifndef PIT_H
#define PIT_H

#include "arch/x86/idt/idt.h"

/**
 * @brief Initialize the Programmable Interval Timer (PIT).
 *
 * This function sets up the PIT to generate interrupts at a regular interval. It configures the PIT's mode and
 * frequency to ensure that it can be used for timing purposes in the operating system. After calling this function, the
 * PIT will start generating interrupts at the configured interval, allowing the system to perform time-related tasks
 * such as scheduling and timekeeping.
 *
 */
void pit_init();

/**
 * @brief Handle the PIT timer interrupt.
 *
 * This function is called when the PIT timer generates an interrupt. It should perform any necessary actions to handle
 * the timer event, such as updating system time, scheduling tasks, or performing other time-related operations.
 *
 * @param frame A pointer to the interrupt frame containing the state of the CPU at the time of the interrupt.
 */
void pit_handler(const isr_frame_t* frame);

/**
 * @brief Wait for a specified number of milliseconds using the PIT timer.
 *
 * This function will block the calling thread until the specified number of milliseconds has elapsed. It uses the PIT
 * timer to achieve accurate timing. Make sure to call `pit_init()` before using this function to ensure the PIT timer
 * is properly configured.
 *
 * @param milliseconds The number of milliseconds to wait.
 */
void pit_wait(uint32_t milliseconds);

#endif // PIT_H
