#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/**
 * @brief Represents the CPU state and metadata at the time of an interrupt or exception.
 *
 * This structure captures the state of the CPU registers and relevant information when an interrupt or exception
 * occurs. It includes general-purpose registers, segment selectors, instruction pointer, flags, and other context
 * needed for handling the interrupt. The layout of this structure is designed to match the order in which the CPU and
 * the interrupt service routine (ISR) stubs push data onto the stack during an interrupt event. This allows the ISR
 * handlers to access the CPU state and interrupt information in a consistent manner, regardless of the specific type of
 * interrupt or exception being handled.
 */
typedef struct {
    /** Pushed manually in isr_common_stub */
    uint32_t cr2, ds;

    /** Pushed by the pusha instruction */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;

    /** Pushed manually by the stub (e.g., isr_stub_0) */
    uint32_t int_no, err_code;

    /** Pushed automatically by the CPU */
    uint32_t eip, cs, eflags;

    /** Pushed if the ISR came from user mode (Ring 3) */
    uint32_t user_esp, ss;
} __attribute__((packed)) isr_frame_t;

/**
 * @brief Enumeration of x86 exception vector numbers.
 *
 * This enumeration defines the standard exception vector numbers for the x86 architecture. Each entry corresponds to a
 * specific exception that can occur during the execution of a program, such as division by zero, page faults, and
 * general protection faults. These vector numbers are used to identify the type of exception that has occurred and to
 * dispatch the appropriate handler when an exception is raised. The values in this enumeration are based on the
 * standard x86 architecture specifications for exceptions.
 */
typedef enum {
    EXCEPTION_DIVIDE_BY_ZERO = 0,
    EXCEPTION_DEBUG,
    EXCEPTION_NON_MASKABLE_INTERRUPT,
    EXCEPTION_BREAKPOINT,
    EXCEPTION_INTO_DETECTED_OVERFLOW,
    EXCEPTION_OUT_OF_BOUNDS,
    EXCEPTION_INVALID_OPCODE,
    EXCEPTION_NO_COPROCESSOR,
    EXCEPTION_DOUBLE_FAULT,
    EXCEPTION_COPROCESSOR_SEGMENT_OVERRUN,
    EXCEPTION_BAD_TSS,
    EXCEPTION_SEGMENT_NOT_PRESENT,
    EXCEPTION_STACK_FAULT,
    EXCEPTION_GENERAL_PROTECTION_FAULT,
    EXCEPTION_PAGE_FAULT,
    EXCEPTION_UNKNOWN_INTERRUPT,
    EXCEPTION_COPROCESSOR_FAULT,
    EXCEPTION_ALIGNMENT_FAULT,
    EXCEPTION_MACHINE_CHECK,
    EXCEPTION_RESERVED_19,
    EXCEPTION_RESERVED_20,
    EXCEPTION_RESERVED_21,
    EXCEPTION_RESERVED_22,
    EXCEPTION_RESERVED_23,
    EXCEPTION_RESERVED_24,
    EXCEPTION_RESERVED_25,
    EXCEPTION_RESERVED_26,
    EXCEPTION_RESERVED_27,
    EXCEPTION_RESERVED_28,
    EXCEPTION_RESERVED_29,
    EXCEPTION_RESERVED_30,
    EXCEPTION_RESERVED_31
} isr_exception_t;

/**
 * @brief Represents a pointer to the Interrupt Descriptor Table (IDT) for x86 architecture.
 *
 * This structure defines the layout of the IDT pointer, which includes the limit (size) of the IDT and the base address
 * where the IDT is located in memory.
 */
typedef struct {
    uint16_t limit; /*< The size of the IDT in bytes minus one */
    uint32_t base;  /*< The base address of the IDT in memory */
} __attribute__((packed)) idt_ptr_t;

/**
 * @brief Represents an entry in the Interrupt Descriptor Table (IDT) for x86 architecture.
 *
 * This structure defines the layout of an IDT entry, which includes the address of the interrupt service routine (ISR),
 * the segment selector, and various attributes that specify the type and properties of the interrupt.
 */
typedef struct {
    uint16_t isr_low;   /**< The lower 16bit of the ISRs address */
    uint16_t selector;  /**< The GDT segment selector that the CPU will load into CS before calling the ISR */
    uint8_t zero;       /**< Some reserved bits that are always set to zero */
    uint8_t attributes; /**< Types and attributes */
    uint16_t isr_high;  /**< The higher 16bit of the ISRs address */
} __attribute__((packed)) idt_entry_t;

// Type definition for a ISR handler function pointer
typedef void (*isr_t)(const isr_frame_t* frame);

// Type definition for a IRQ handler function pointer
typedef void (*irq_t)(const isr_frame_t* frame);

/**
 * @brief Initializes the x86 Interrupt Descriptor Table (IDT).
 *
 * Sets up the IDT entries and loads the IDT register so the CPU can dispatch interrupts and exceptions to their
 * corresponding handlers.
 *
 * @note This function is expected to be called during early kernel startup before enabling interrupts.
 */
void idt_init();

/**
 * @brief Installs an interrupt service routine (ISR) handler.
 *
 * Registers an ISR handler for the specified interrupt vector. This function configures the IDT (Interrupt Descriptor
 * Table) entry to point to the provided handler function, allowing the system to respond to the corresponding
 * interrupt.
 *
 * @param isr     The interrupt vector number (0-255) to install the handler for.
 * @param handler A function pointer to the ISR handler to be called when the interrupt is triggered. The handler should
 *                be of type isr_t.
 *
 * @note This function should only be called during system initialization or when it is safe to modify interrupt
 *       handling behavior.
 */
void isr_install(uint8_t isr, isr_t handler);

/**
 * @brief Uninstalls an interrupt service routine (ISR) from the interrupt descriptor table (IDT).
 *
 * This function removes the ISR handler at the specified index, effectively disabling interrupt handling for that
 * particular interrupt vector. After uninstallation, interrupts at this vector will no longer be processed by the
 * previously installed handler.
 *
 * @param isr The index of the ISR to uninstall from the IDT. This should be a valid IDT entry number (typically 0-255
 *            for x86 architecture).
 */
void isr_uninstall(uint8_t isr);

/**
 * @brief Handles an interrupt service routine (ISR) event.
 *
 * This function is invoked when an interrupt or exception is dispatched to the common ISR handler. It receives a
 * snapshot of the CPU state at the time of the event and performs architecture-specific interrupt handling logic.
 *
 * @param frame Pointer to an immutable ISR frame containing saved processor context and interrupt metadata.
 */
void isr_handler(const isr_frame_t* frame);

/**
 * @brief Registers an interrupt request (IRQ) handler for a specific IRQ line.
 *
 * Installs the provided callback so it is invoked when the corresponding hardware IRQ is triggered.
 *
 * @param irq     The IRQ number to bind the handler to.
 * @param handler The function to execute when the IRQ occurs.
 */
void irq_install(uint8_t irq, irq_t handler);

/**
 * @brief Uninstalls an interrupt request (IRQ) handler for a specific IRQ line.
 *
 * Removes the handler associated with the given IRQ number, effectively disabling the response to that hardware
 * interrupt.
 *
 * @param irq The IRQ number for which to remove the handler.
 */
void irq_uninstall(uint8_t irq);

/**
 * @brief Handles an interrupt request (IRQ) event.
 *
 * This function is invoked when an IRQ is received. It processes the interrupt and calls the appropriate handler if one
 * has been installed.
 *
 * @param frame Pointer to an immutable ISR frame containing saved processor context and interrupt metadata.
 */
void irq_handler(const isr_frame_t* frame);

/**
 * @brief Creates an IDT entry for the specified interrupt vector.
 *
 * This function constructs an IDT entry that points to the given interrupt service routine (ISR)
 * and sets the appropriate attributes for the entry. The resulting IDT entry can then be used to populate the IDT,
 * allowing the CPU to correctly dispatch interrupts to the specified ISR.
 *
 * @param isr         The interrupt vector number (0-255).
 * @param attributes  The attributes for the IDT entry.

 * @return A fully populated `idt_entry_t` structure representing the IDT entry for the specified ISR and attributes.
 */
idt_entry_t idt_create_entry(uint32_t isr, uint8_t attributes);

/**
 * @brief Creates an IDT pointer for the specified number of entries and base address.
 *
 * This function constructs an `idt_ptr_t` structure that describes the location and size of the IDT. The `limit` field
 * is calculated based on the number of entries and the size of each entry, while the `base` field is set to the
 * provided base address of the IDT. This structure can then be used to load the IDT register with the correct
 * information about the IDT's location and size, allowing the CPU to properly access the IDT when handling interrupts.
 *
 * @param entries The number of IDT entries.
 * @param base    The base address of the IDT.

 * @return A populated `idt_ptr_t` structure describing the IDT location and size.
 */
idt_ptr_t idt_create_ptr(uint16_t entries, uint32_t base);

/**
 * @brief Loads the IDT using the provided IDT pointer.
 *
 * This function uses the `lidt` instruction to load the IDT register with the address and size specified in the
 * `idt_ptr_t` structure. After calling this function, the CPU will use the new IDT for handling interrupts and
 * exceptions.
 */
extern void idt_load(idt_ptr_t* idt_ptr);

#endif // IDT_H
