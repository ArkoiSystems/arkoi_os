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

# The bootstrap entry point runs in lower-half physical memory.
.section .bootstrap.text, "ax"

.global _start
.type _start, @function
_start:
    # Verify Multiboot2 handoff
    cmpl $MB2_BOOT_MAGIC, %eax
    jne hang_boot

    # Preserve the pointer to the Multiboot2 info structure in %esi for later use
    mov %ebx, %esi

    # Use a physical stack until higher-half paging is turned on.
    mov $stack_top, %esp
    sub $_kernel_virtual_base, %esp

    # Bootstrap page tables in C (called through low alias before paging is enabled).
    mov $vmm_preinit, %eax
    sub $_kernel_virtual_base, %eax
    call *%eax

    lea higher_half_entry, %eax
    jmp *%eax

hang_boot:
    cli
    hlt
    jmp hang_boot

# Continue execution in higher-half virtual memory after paging is enabled.
.section .text
higher_half_entry:
    mov $stack_top, %esp

    # Call the global constructors
    call _init

    # Set up the GDT and IDT
    call gdt_init
    call idt_init

    # Initialize the virtual memory manager (VMM)
    call vmm_init

    # Transfer control to the kernel and pass Multiboot2 info pointer as first arg
    push %esi
    call kernel_main
    add $4, %esp

    # Hang if kernel_main unexpectedly returns
hang:
    cli
    hlt
	jmp hang
