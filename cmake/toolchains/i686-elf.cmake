# Toolchain file for building Arkoi kernel for i686-elf target

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR i686)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TOOLS "$ENV{TOOLS}" CACHE PATH "Path to cross toolchain")

if(NOT TOOLS)
    set(TOOLS "$ENV{HOME}/opt/cross" CACHE PATH "Fallback toolchain path")
endif()

set(CMAKE_FIND_ROOT_PATH "${TOOLS}")

set(CMAKE_C_COMPILER i686-elf-gcc)
set(CMAKE_CXX_COMPILER i686-elf-g++)
set(CMAKE_ASM_COMPILER i686-elf-as)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)