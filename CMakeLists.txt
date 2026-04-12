cmake_minimum_required(VERSION 3.28)

project(arkoi_workspace LANGUAGES NONE)

include(ExternalProject)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(ARKOI_ENABLE_KERNEL "Enable freestanding kernel build" ON)
option(ARKOI_ENABLE_TESTS "Enable hosted unit test build" ON)

set(ARKOI_KERNEL_BUILD_DIR "${CMAKE_BINARY_DIR}/kernel")
set(ARKOI_TESTS_BUILD_DIR "${CMAKE_BINARY_DIR}/tests")

set(ARKOI_TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/cmake/toolchains")
set(ARKOI_KERNEL_TOOLCHAIN_FILE "${ARKOI_TOOLCHAIN_DIR}/i686-elf.cmake")
set(ARKOI_HOST_TOOLCHAIN_FILE "${ARKOI_TOOLCHAIN_DIR}/host.cmake")

function(copy_compile_commands target_name build_dir output_path)
    ExternalProject_Add_Step(${target_name} copy_compile_commands
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${build_dir}/compile_commands.json"
            "${output_path}"
        DEPENDEES build
        ALWAYS FALSE
    )
endfunction()

add_custom_target(arkoi_workspace ALL
    COMMENT "Build selected Arkoi components"
)

if(ARKOI_ENABLE_KERNEL)
    ExternalProject_Add(kernel
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/kernel"
        BINARY_DIR "${ARKOI_KERNEL_BUILD_DIR}"
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_TOOLCHAIN_FILE=${ARKOI_KERNEL_TOOLCHAIN_FILE}
        BUILD_ALWAYS FALSE
        INSTALL_COMMAND ""
        STEP_TARGETS build
    )

    copy_compile_commands(
        kernel
        "${ARKOI_KERNEL_BUILD_DIR}"
        "${CMAKE_SOURCE_DIR}/kernel/compile_commands.json"
    )
    
    add_dependencies(arkoi_workspace kernel)
endif()

if(ARKOI_ENABLE_TESTS)
    ExternalProject_Add(host_tests
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/tests"
        BINARY_DIR "${ARKOI_TESTS_BUILD_DIR}"
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_TOOLCHAIN_FILE=${ARKOI_HOST_TOOLCHAIN_FILE}
        BUILD_ALWAYS FALSE
        INSTALL_COMMAND ""
        TEST_COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        STEP_TARGETS build
    )

    copy_compile_commands(
        host_tests
        "${ARKOI_TESTS_BUILD_DIR}"
        "${CMAKE_SOURCE_DIR}/tests/compile_commands.json"
    )
    
    add_dependencies(arkoi_workspace host_tests)
endif()
