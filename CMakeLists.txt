cmake_minimum_required(VERSION 3.30.2)
project(ArkoiOS)

# Enable assembly language support
enable_language(ASM)

# System target
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR i686)

# Cross compiler toolchain
set(TOOLS $ENV{HOME}/opt/cross)
set(CMAKE_C_COMPILER ${TOOLS}/bin/i686-elf-gcc)
set(CMAKE_CXX_COMPILER ${TOOLS}/bin/i686-elf-g++)
set(CMAKE_ASM_COMPILER ${TOOLS}/bin/i686-elf-as)

# Compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_FLAGS "-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti")
set(CMAKE_C_FLAGS "-ffreestanding -O2 -Wall -Wextra")

# Kernel binary and ISO
set(KERNEL_BIN ${CMAKE_PROJECT_NAME}.bin)
set(KERNEL_ISO ${CMAKE_PROJECT_NAME}.iso)

# Create the executable using the linker script
include_directories(include)

add_executable(
    ${KERNEL_BIN}
    src/kernel.c
    src/boot.s
    src/gdt/gdt.s
    src/gdt/gdt.c
    src/lib/kstdio.c
    src/lib/kpanic.c
    src/lib/kstring.c
    src/lib/kmemory.c
    src/drivers/vga.c
)
set_target_properties(${KERNEL_BIN} PROPERTIES LINK_FLAGS "-T ${CMAKE_SOURCE_DIR}/linker.ld -ffreestanding -O2 -nostdlib -lgcc")

# Copy the output binary into the bin directory
add_custom_command(
    TARGET ${KERNEL_BIN} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy ${KERNEL_BIN} ${CMAKE_SOURCE_DIR}/bin/
)