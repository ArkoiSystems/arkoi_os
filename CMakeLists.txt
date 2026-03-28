cmake_minimum_required(VERSION 3.28)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR i686)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(ArkoiOS LANGUAGES C ASM)

# Enable ASM explicitly (safer)
enable_language(ASM)

# =========================
# Toolchain Detection
# =========================
set(TOOLS "$ENV{TOOLS}" CACHE PATH "Path to cross toolchain")

if(NOT TOOLS)
    set(TOOLS "$ENV{HOME}/opt/cross" CACHE PATH "Fallback toolchain path")
endif()

# Prefer explicit toolchain path
if(EXISTS "${TOOLS}/bin/i686-elf-gcc")
    set(CMAKE_C_COMPILER "${TOOLS}/bin/i686-elf-gcc" CACHE FILEPATH "")
    set(CMAKE_ASM_COMPILER "${TOOLS}/bin/i686-elf-as" CACHE FILEPATH "")
else()
    find_program(I686_ELF_GCC i686-elf-gcc REQUIRED)
    find_program(I686_ELF_AS  i686-elf-as  REQUIRED)

    set(CMAKE_C_COMPILER "${I686_ELF_GCC}" CACHE FILEPATH "")
    set(CMAKE_ASM_COMPILER "${I686_ELF_AS}" CACHE FILEPATH "")
endif()

message(STATUS "Using C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Using ASM compiler: ${CMAKE_ASM_COMPILER}")

# =========================
# Standards & Flags
# =========================
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Common flags
set(COMMON_FLAGS
    -ffreestanding
    -Wall
    -Wextra
)

# Debug vs Release
set(CMAKE_C_FLAGS_DEBUG   "-O0 -g")
set(CMAKE_C_FLAGS_RELEASE "-O2")

# =========================
# Kernel Target
# =========================
set(KERNEL_BIN ${PROJECT_NAME}.bin)

set(SOURCES
    src/kernel.c

    src/gdt/gdt.s
    src/gdt/gdt.c

    src/idt/idt.s
    src/idt/idt.c
    src/idt/pic.c

    src/lib/kstdio.c
    src/lib/kpanic.c
    src/lib/kstring.c
    src/lib/kmemory.c
    src/lib/kio.c

    src/drivers/vga.c
    src/drivers/keyboard.c
    src/drivers/pit.c

    src/boot/crtn.s
    src/boot/crti.s
    src/boot/boot.s
)

add_executable(${KERNEL_BIN} ${SOURCES})

target_include_directories(${KERNEL_BIN}
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)

target_compile_options(${KERNEL_BIN}
    PRIVATE ${COMMON_FLAGS}
)

target_link_options(${KERNEL_BIN}
    PRIVATE
        -T${CMAKE_SOURCE_DIR}/linker.ld
        -nostdlib
        -ffreestanding
)

target_link_libraries(${KERNEL_BIN}
    PRIVATE gcc
)