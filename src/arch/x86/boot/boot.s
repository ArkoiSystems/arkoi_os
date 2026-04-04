# Declare constants for the multiboot header
.set MB_ALIGN,    1<<0                   # Align loaded modules on page boundaries
.set MB_MEMINFO,  1<<1                   # Provide memory map
.set MB_FLAGS,    MB_ALIGN | MB_MEMINFO  # This is the Multiboot 'flag' field
.set MB_MAGIC,    0x1BADB002             # The 'magic number' lets bootloader find the header
.set MB_CHECKSUM, -(MB_MAGIC + MB_FLAGS) # Checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard
.section .multiboot
.align 4
.long MB_MAGIC
.long MB_FLAGS
.long MB_CHECKSUM

# Reserve a stack for the initial thread
.section .bss
.align 16
stack_bottom:
.skip 4096 # 4 KiB
stack_top:

# The kernel entry point
.section .text

.global _start
.type _start, @function
_start:
    mov $stack_top, %esp

    # Call the global constructors
    call _init

    # Transfer the control to the main kernel
    call kernel_main

    # Hang if kernel_main unexpectedly returns
hang:
    cli
    hlt
	jmp hang

# Set the size of the _start symbol to the current location '.' minus its start.
# This is useful when debugging or when you implement call tracing.
.size _start, . - _start
