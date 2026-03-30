# Arkoi Operating System

Arkoi is an educational 32-bit operating system kernel in C17 and x86 assembly. The codebase focuses on core kernel primitives (boot, GDT/IDT, IRQ handling, text output, and low-level utility code) while keeping module boundaries explicit and easy to extend.

## Project Layout

```text
include/
	arch/x86/        # x86-specific public interfaces (GDT, IDT, PIC)
	drivers/         # device-facing public interfaces
	lib/             # kernel utility interfaces

src/
	arch/x86/
		boot/          # multiboot entry and CRT init/fini
		gdt/           # descriptor table setup + asm loader
		idt/           # interrupt tables, stubs, PIC glue
	drivers/         # VGA, keyboard, PIT
	kernel/          # kernel entrypoint and high-level init flow
	lib/             # low-level kernel utility implementations
```

## Build

Requirements:
- i686-elf cross toolchain on PATH, or set `TOOLS` to your toolchain root
- CMake 3.28+
- Ninja or another supported CMake generator

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Run

```bash
./start.sh
```

Optional:
- Override kernel path: `KERNEL_BIN=build/ArkoiOS.bin ./start.sh`
- Pass QEMU flags directly: `./start.sh -serial stdio`

## Extending The Project

- Add architecture-specific code under `src/arch/<arch>` and public interfaces under `include/arch/<arch>`.
- Keep driver code in `src/drivers` and expose only necessary interfaces from `include/drivers`.
- Register new sources in `CMakeLists.txt` using the grouped source lists (`ARCH_SOURCES`, `DRIVER_SOURCES`, `LIB_SOURCES`, `KERNEL_SOURCES`).
- Select architecture through `ARKOI_ARCH` cache variable (currently supports `x86`).

## Credits

This repository uses the following open source projects:
- [OSDev-Clion](https://github.com/balsigergil/OSDev-Clion)
- [OSDev Wiki](https://wiki.osdev.org/)
- [OliveStem x86 OS - Playlist](https://www.youtube.com/playlist?list=PL2EF13wm-hWAglI8rRbdsCPq_wRpYvQQy)

## License

This project is licensed under the BSD 3-Clause License. See [LICENSE](LICENSE) for details.
