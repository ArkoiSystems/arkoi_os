# Toolchain file for building unit tests on the host machine

if(NOT CMAKE_C_COMPILER)
    find_program(ARKOI_HOST_CC NAMES gcc clang cc REQUIRED)
    set(CMAKE_C_COMPILER "${ARKOI_HOST_CC}" CACHE FILEPATH "" FORCE)
endif()

if(NOT CMAKE_CXX_COMPILER)
    find_program(ARKOI_HOST_CXX NAMES g++ clang++ c++ REQUIRED)
    set(CMAKE_CXX_COMPILER "${ARKOI_HOST_CXX}" CACHE FILEPATH "" FORCE)
endif()

if(NOT CMAKE_ASM_COMPILER)
    find_program(ARKOI_HOST_AS NAMES as nasm yasm REQUIRED)
    set(CMAKE_ASM_COMPILER "${ARKOI_HOST_AS}" CACHE FILEPATH "" FORCE)
endif()