cmake_minimum_required(VERSION 3.28)

project(arkoi_tests LANGUAGES C CXX)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(ARKOI_KERNEL_ROOT "${CMAKE_CURRENT_LIST_DIR}/../kernel")

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(INSTALL_GTEST OFF CACHE BOOL "Disable GoogleTest install")

FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_executable(arkoi_tests
    src/kbuffer_tests.cpp
    "${ARKOI_KERNEL_ROOT}/src/lib/kstring.c"
    "${ARKOI_KERNEL_ROOT}/src/lib/kmemory.c"
    "${ARKOI_KERNEL_ROOT}/src/lib/kbuffer.c"
)

target_include_directories(arkoi_tests
    PRIVATE "${ARKOI_KERNEL_ROOT}/include"
)

target_compile_options(arkoi_tests
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_link_libraries(arkoi_tests
    PRIVATE
        GTest::gtest_main
)

gtest_discover_tests(arkoi_tests)