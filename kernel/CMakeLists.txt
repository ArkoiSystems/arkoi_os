cmake_minimum_required(VERSION 3.28)

project(arkoi_kernel LANGUAGES C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(COMMON_C_FLAGS
    -ffreestanding
    -Wall
    -Wextra
)

set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
set(CMAKE_C_FLAGS_RELEASE "-O2")

set(ARKOI_ARCH "x86" CACHE STRING "Target architecture")
set_property(CACHE ARKOI_ARCH PROPERTY STRINGS x86)

if(NOT ARKOI_ARCH STREQUAL "x86")
    message(FATAL_ERROR "Unsupported ARKOI_ARCH='${ARKOI_ARCH}'. Supported architectures: x86")
endif()

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
    src/arch/x86/boot/multiboot2.c
)

set(DRIVER_SOURCES
    src/drivers/vga.c
    src/drivers/keyboard.c
    src/drivers/pit.c
    src/drivers/serial.c
)

set(LIB_SOURCES
    src/lib/kcursor.c
    src/lib/kstdio.c
    src/lib/kpanic.c
    src/lib/kstring.c
    src/lib/kmemory.c
    src/lib/memory/emm.c
    src/lib/memory/pmm.c
    src/lib/memory/vmm.c
    src/lib/kbuffer.c
    src/lib/kio.c
)

add_executable(${KERNEL_BIN}
    ${KERNEL_SOURCES}
    ${ARCH_SOURCES}
    ${DRIVER_SOURCES}
    ${LIB_SOURCES}
)

target_include_directories(${KERNEL_BIN}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(${KERNEL_BIN}
    PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${COMMON_C_FLAGS}>
)

target_link_options(${KERNEL_BIN}
    PRIVATE
    -T${PROJECT_SOURCE_DIR}/../linker.ld
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

set(ISO_NAME "${PROJECT_NAME}.iso")
set(ISO_PATH "${CMAKE_BINARY_DIR}/${ISO_NAME}")
set(ISO_ROOT "${CMAKE_BINARY_DIR}/isodir")
set(GRUB_CFG_SOURCE "${PROJECT_SOURCE_DIR}/../grub/grub.cfg")

add_custom_target(iso ALL
    DEPENDS ${KERNEL_BIN}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ISO_ROOT}/boot/grub"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${KERNEL_BIN}>" "${ISO_ROOT}/boot/${KERNEL_BIN}"
    COMMAND ${CMAKE_COMMAND} -E copy "${GRUB_CFG_SOURCE}" "${ISO_ROOT}/boot/grub/grub.cfg"
    COMMAND grub-mkrescue -o "${ISO_PATH}" "${ISO_ROOT}"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMENT "Building bootable GRUB ISO: ${ISO_PATH}"
    VERBATIM
)
