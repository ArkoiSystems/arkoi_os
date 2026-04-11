cmake_minimum_required(VERSION 3.28)

project(ArkoiOSHostTests LANGUAGES C CXX)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(INSTALL_GTEST OFF CACHE BOOL "Disable GoogleTest install")
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_library(arkoi_kernel_components STATIC
    ../kernel/src/lib/kstring.c
    ../kernel/src/lib/kmemory.c
)

target_include_directories(arkoi_kernel_components
    PUBLIC ../kernel/include
)

target_compile_options(arkoi_kernel_components
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

add_executable(arkoi_kernel_unit_tests
    main.cpp
)

target_link_libraries(arkoi_kernel_unit_tests
    PRIVATE
        arkoi_kernel_components
        GTest::gtest_main
)

gtest_discover_tests(arkoi_kernel_unit_tests)