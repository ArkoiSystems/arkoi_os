# Declare constants for the Multiboot2 header
.set MB2_MAGIC,        0xE85250D6
.set MB2_ARCH,         0
.set MB2_HEADER_LEN,   24
.set MB2_CHECKSUM,     -(MB2_MAGIC + MB2_ARCH + MB2_HEADER_LEN)
.set MB2_BOOT_MAGIC,   0x36D76289

# Declare a Multiboot2 header. The mandatory end tag has type=0, size=8.
.section .multiboot.data, "aw"
.align 8
.long MB2_MAGIC
.long MB2_ARCH
.long MB2_HEADER_LEN
.long MB2_CHECKSUM
.short 0
.short 0
.long 8

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

    # Verify Multiboot2 handoff
    cmpl $MB2_BOOT_MAGIC, %eax
    jne hang
    # Preserve the pointer to the Multiboot2 info structure in %esi for later use
    mov %ebx, %esi

    # Call the global constructors
    call _init

    # Set up the GDT and IDT
    call gdt_initialize
    call idt_initialize

    # Initialize the virtual memory manager (VMM) and set up identity paging
    call vmm_initialize

    # Transfer control to the kernel and pass Multiboot2 info pointer as first arg
    push %esi
    call kernel_main
    add $4, %esp

    # Hang if kernel_main unexpectedly returns
hang:
    cli
    hlt
	jmp hang

# Set the size of the _start symbol to the current location '.' minus its start.
# This is useful when debugging or when you implement call tracing.
.size _start, . - _start
