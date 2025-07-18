.intel_syntax noprefix

# Segment Selectors:
# https://wiki.osdev.org/Segment_Selector
# Bit-Format: 15-3=Index, 2=TI, 1-0=RPL
# - Selector 0x10:
#   - Index = 10b (2, Kernel Data)
#   - TI = 0 (0=GDT, 1=LDT)
#   - RPL = 0 (0=Kernel, 3=User)
# - Selector 0x08:
#   - Index = 01b (1, Kernel Code)
#   - TI = 0 (0=GDT, 1=LDT)
#   - RPL = 0 (0=Kernel, 3=User)

.global gdt_load
gdt_load:
    mov eax, [esp + 4] # Move the gdt_ptr_t address from stack into eax
    lgdt [eax]         # Load the gdt_ptrt_t at [eax]

    # Set all the segment registers (ds, es, fs, gs, ss) to point to the kernel data (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    # Also set the cs by doing a far jump into the kernel code (0x08)
    jmp 0x08:.flush_cs
.flush_cs:
    ret
