cmake_minimum_required(VERSION 3.28)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR i686)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(ArkoiOS LANGUAGES C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
# Target Architecture
# =========================
set(ARKOI_ARCH "x86" CACHE STRING "Target architecture")
set_property(CACHE ARKOI_ARCH PROPERTY STRINGS x86)

if(NOT ARKOI_ARCH STREQUAL "x86")
    message(FATAL_ERROR "Unsupported ARKOI_ARCH='${ARKOI_ARCH}'. Supported architectures: x86")
endif()

# =========================
# Kernel Target
# =========================
set(KERNEL_BIN ${PROJECT_NAME}.bin)

set(KERNEL_SOURCES
    src/kernel/main.c
)

set(ARCH_SOURCES
    src/arch/x86/gdt/gdt.s
    src/arch/x86/gdt/gdt.c
    src/arch/x86/idt/idt.s
    src/arch/x86/idt/idt.c
    src/arch/x86/idt/pic.c
    src/arch/x86/boot/crtn.s
    src/arch/x86/boot/crti.s
    src/arch/x86/boot/boot.s
)

set(DRIVER_SOURCES
    src/drivers/vga.c
    src/drivers/keyboard.c
    src/drivers/pit.c
)

set(LIB_SOURCES
    src/lib/kstdio.c
    src/lib/kpanic.c
    src/lib/kstring.c
    src/lib/kmemory.c
    src/lib/kio.c
)

add_executable(${KERNEL_BIN}
    ${KERNEL_SOURCES}
    ${ARCH_SOURCES}
    ${DRIVER_SOURCES}
    ${LIB_SOURCES}
)

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

set_target_properties(${KERNEL_BIN}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)