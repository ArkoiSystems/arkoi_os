# ArkoiOS

ArkoiOS is an educational 32-bit x86 kernel written in C17 and x86 assembly.
This repository demonstrates the early bring-up path of a freestanding kernel: Multiboot2 boot, higher-half paging, descriptor tables, interrupt handling, VGA output, keyboard/PIT wiring, and early memory management.

## What This Repository Is Doing

At a high level, the kernel currently boots and performs this sequence:

1. GRUB loads the kernel via Multiboot2.
2. Early assembly bootstrap verifies Multiboot2 handoff and sets up a temporary stack.
3. Early paging is enabled and execution jumps to higher-half virtual addresses.
4. GDT and IDT are initialized, and PIC IRQ vectors are remapped.
5. The virtual memory manager registers a page-fault ISR.
6. `kernel_main` parses and prints Multiboot2 boot info (loader name, command line, memory map, modules).
7. A physical memory manager is initialized and populated from detected RAM regions.
8. A deliberate page fault is triggered to validate fault handling output.

Because of step 8, the default runtime is currently a memory/exception bring-up demo rather than a full interactive shell.

## Current Behavior In QEMU

On boot, you should see text output on VGA showing:

- Boot loader and command line details
- RAM and reserved region summaries
- Module info (if present)
- A page-fault diagnostic block (fault address, error code, access type, instruction pointer)

After the page fault, execution halts in the exception path.

If you remove the test fault in `src/kernel/main.c`, the next stage initializes PIT and keyboard IRQ handling and echoes pressed keys to the VGA screen in light green.

## Repository Layout

```text
include/
  arch/x86/        # x86 public interfaces (boot, GDT, IDT, PIC)
  drivers/         # keyboard, PIT, VGA interfaces
  lib/             # kernel utility and memory interfaces

src/
  arch/x86/boot/   # Multiboot2 header/entry, CRT, early paging jump
  arch/x86/gdt/    # GDT structures and loader
  arch/x86/idt/    # IDT setup, ISR/IRQ dispatch, PIC integration
  drivers/         # VGA text output, keyboard, PIT
  kernel/          # kernel_main and high-level init sequence
  lib/memory/      # EMM (early allocator), PMM (buddy allocator), VMM
```

## Build

Requirements:

- CMake 3.28+
- Ninja (or another CMake generator)
- `i686-elf-gcc` and `i686-elf-as` on PATH (or set `TOOLS` to your cross-toolchain root)
- `grub-mkrescue`
- `qemu-system-i386`

Build command:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

The build produces:

- `build/arkoi_kernel.bin` (kernel ELF/binary target)
- `build/arkoi_kernel.iso` (bootable GRUB ISO)

## Run

```bash
./start.sh
```

Optional examples:

```bash
ISO_BIN=build/ArkoiOS.iso ./start.sh
./start.sh -serial stdio
```

## Credits

This repository uses ideas and references from:

- [OSDev-Clion](https://github.com/balsigergil/OSDev-Clion)
- [OSDev Wiki](https://wiki.osdev.org/)
- [OliveStem x86 OS Playlist](https://www.youtube.com/playlist?list=PL2EF13wm-hWAglI8rRbdsCPq_wRpYvQQy)

## License

Licensed under BSD 3-Clause. See [LICENSE](LICENSE).
