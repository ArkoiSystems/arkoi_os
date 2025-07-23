.intel_syntax noprefix

.global idt_load
idt_load:
    mov eax, [esp + 4] # Move the idt_ptr_t address from stack into eax
    lidt [eax]         # Load the idt_ptrt_t at [eax]
    sti
    ret

.macro isr_error_stub num
.global isr_stub_\num
isr_stub_\num:
    cli                 # Disable any other interrupt to prevent nesting
    push \num           # The error code is already pushed, only push the interrupt number
    jmp isr_common_stub
.endm

.macro isr_no_error_stub num
.global isr_stub_\num
isr_stub_\num:
    cli                 # Disable any other interrupt to prevent nesting
    push 0              # Push the pseudo error code 0 on the stack
    push \num
    jmp isr_common_stub
.endm

.extern isr_handler
isr_common_stub:
    pusha        # Push all the registers onto the stack
    mov eax, ds  # Move and push the DS register onto the stack
    push eax
    mov eax, cr2 # Move and push the CR2 register onto the stack
    push eax
    mov ax, 0x10 # We want to set all the selectors to the kernel data segment to work in it
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    # This will call the ISR Handler which now can access all registers etc using the
    # provided struct with the contents pushed onto the stack.
    push esp         # Pushes the ESP onto the stack (isr_frame_t *)
    call isr_handler

    add esp, 8 # Skip the last two pushes to pop the DS content from the stack
    pop ebx    # Reset all the selectors with the old content of the DS register
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa       # Restore all other registers
    add esp, 8 # We pushed an error code and interrupt number onto the stack, so remove them

    sti        # Enable interrupts again and return from the current one
    iret

.macro irq_stub num int_no
.global irq_stub_\num
irq_stub_\num:
    cli                 # Disable any other interrupt to prevent nesting
    push 0              # Push the pseudo error code 0 on the stack
    push \int_no
    jmp irq_common_stub
.endm

.extern irq_handler
irq_common_stub:
    pusha        # Push all the registers onto the stack
    mov eax, ds  # Move and push the DS register onto the stack
    push eax
    mov eax, cr2 # Move and push the CR2 register onto the stack
    push eax
    mov ax, 0x10 # We want to set all the selectors to the kernel data segment to work in it
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    # This will call the IRQ Handler which now can access all registers etc using the
    # provided struct with the contents pushed onto the stack.
    push esp         # Pushes the ESP onto the stack (isr_frame_t *)
    call irq_handler

    add esp, 8 # Skip the last two pushes to pop the DS content from the stack
    pop ebx    # Reset all the selectors with the old content of the DS register
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa       # Restore all other registers
    add esp, 8 # We pushed an error code and interrupt number onto the stack, so remove them

    sti        # Enable interrupts again and return from the current one
    iret

isr_no_error_stub  0
isr_no_error_stub  1
isr_no_error_stub  2
isr_no_error_stub  3
isr_no_error_stub  4
isr_no_error_stub  5
isr_no_error_stub  6
isr_no_error_stub  7
isr_error_stub     8
isr_no_error_stub  9
isr_error_stub    10
isr_error_stub    11
isr_error_stub    12
isr_error_stub    13
isr_error_stub    14
isr_no_error_stub 15
isr_no_error_stub 16
isr_error_stub    17
isr_no_error_stub 18
isr_no_error_stub 19
isr_no_error_stub 20
isr_no_error_stub 21
isr_no_error_stub 22
isr_no_error_stub 23
isr_no_error_stub 24
isr_no_error_stub 25
isr_no_error_stub 26
isr_no_error_stub 27
isr_no_error_stub 28
isr_no_error_stub 29
isr_error_stub    30
isr_no_error_stub 31

irq_stub       0, 32
irq_stub       1, 33
irq_stub       2, 34
irq_stub       3, 35
irq_stub       4, 36
irq_stub       5, 37
irq_stub       6, 38
irq_stub       7, 39
irq_stub       8, 40
irq_stub       9, 41
irq_stub      10, 42
irq_stub      11, 43
irq_stub      12, 44
irq_stub      13, 45
irq_stub      14, 46
irq_stub      15, 47

.global isr_stub_table
isr_stub_table:
    .long isr_stub_0
    .long isr_stub_1
    .long isr_stub_2
    .long isr_stub_3
    .long isr_stub_4
    .long isr_stub_5
    .long isr_stub_6
    .long isr_stub_7
    .long isr_stub_8
    .long isr_stub_9
    .long isr_stub_10
    .long isr_stub_11
    .long isr_stub_12
    .long isr_stub_13
    .long isr_stub_14
    .long isr_stub_15
    .long isr_stub_16
    .long isr_stub_17
    .long isr_stub_18
    .long isr_stub_19
    .long isr_stub_20
    .long isr_stub_21
    .long isr_stub_22
    .long isr_stub_23
    .long isr_stub_24
    .long isr_stub_25
    .long isr_stub_26
    .long isr_stub_27
    .long isr_stub_28
    .long isr_stub_29
    .long isr_stub_30
    .long isr_stub_31

.global irq_stub_table
irq_stub_table:
    .long irq_stub_0
    .long irq_stub_1
    .long irq_stub_2
    .long irq_stub_3
    .long irq_stub_4
    .long irq_stub_5
    .long irq_stub_6
    .long irq_stub_7
    .long irq_stub_8
    .long irq_stub_9
    .long irq_stub_10
    .long irq_stub_11
    .long irq_stub_12
    .long irq_stub_13
    .long irq_stub_14
    .long irq_stub_15