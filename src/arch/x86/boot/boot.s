# Declare constants for the Multiboot2 header
.set MB2_MAGIC,        0xE85250D6
.set MB2_ARCH,         0
.set MB2_HEADER_LEN,   24
.set MB2_CHECKSUM,     -(MB2_MAGIC + MB2_ARCH + MB2_HEADER_LEN)
.set MB2_BOOT_MAGIC,   0x36D76289

# Declare a Multiboot2 header. The mandatory end tag has type=0, size=8.
.section .multiboot, "aw"
.align 8
.long MB2_MAGIC
.long MB2_ARCH
.long MB2_HEADER_LEN
.long MB2_CHECKSUM
.short 0
.short 0
.long 8

# Reserve early paging structures used before C runtime initialization
.section .bss
.align 4096
boot_page_directory:
.skip 4096 # 1 page directory

.align 4096
boot_page_table:
.skip 4096 # 1 page table

# The kernel entry point
.section .text

.global _start
.type _start, @function
_start:
    # Verify Multiboot2 handoff
    cmpl $MB2_BOOT_MAGIC, %eax
    jne hang

    # Preserve the pointer to the Multiboot2 info structure in %esi for later use
    mov %ebx, %esi

    # Use a physical stack until higher-half paging is turned on.
    mov $stack_top, %esp
    sub $_kernel_virtual_base, %esp

    # Bootstrap page tables in C (called through low alias before paging is enabled).
    mov $boot_page_table, %eax
    sub $_kernel_virtual_base, %eax
    push %eax
    mov $boot_page_directory, %eax
    sub $_kernel_virtual_base, %eax
    push %eax
    mov $vmm_enable_paging, %eax
    sub $_kernel_virtual_base, %eax
    call *%eax
    add $8, %esp

    lea higher_half_entry, %eax
    jmp *%eax

higher_half_entry:
    mov $stack_top, %esp

    # Call the global constructors
    call _init

    # Set up the GDT and IDT
    call gdt_initialize
    call idt_initialize

    # Initialize the virtual memory manager (VMM)
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
