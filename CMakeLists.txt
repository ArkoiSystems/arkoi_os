cmake_minimum_required(VERSION 3.28)

project(ArkoiWorkspace LANGUAGES NONE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(ARKOI_ENABLE_KERNEL "Enable freestanding kernel build" ON)
option(ARKOI_ENABLE_HOST_TESTS "Enable hosted unit test build" ON)

set(ARKOI_KERNEL_BUILD_DIR "${CMAKE_BINARY_DIR}/kernel")
set(ARKOI_TESTS_BUILD_DIR "${CMAKE_BINARY_DIR}/tests")

find_program(HOST_CC gcc REQUIRED)
find_program(HOST_CXX g++ REQUIRED)

add_custom_target(arkoi_workspace ALL
    COMMENT "Build selected Arkoi components"
)

if(ARKOI_ENABLE_KERNEL)
    add_custom_target(kernel
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${ARKOI_KERNEL_BUILD_DIR}"
        COMMAND ${CMAKE_COMMAND} -S "${CMAKE_SOURCE_DIR}/kernel" -B "${ARKOI_KERNEL_BUILD_DIR}" -G "${CMAKE_GENERATOR}" -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        COMMAND ${CMAKE_COMMAND} --build "${ARKOI_KERNEL_BUILD_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ARKOI_KERNEL_BUILD_DIR}/compile_commands.json" "${CMAKE_SOURCE_DIR}/kernel/compile_commands.json"
        USES_TERMINAL
        COMMENT "Configure and build freestanding kernel project"
        VERBATIM
    )
    add_dependencies(arkoi_workspace kernel)
endif()

if(ARKOI_ENABLE_HOST_TESTS)
    add_custom_target(host_tests
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${ARKOI_TESTS_BUILD_DIR}"
        COMMAND ${CMAKE_COMMAND} -S "${CMAKE_SOURCE_DIR}/tests" -B "${ARKOI_TESTS_BUILD_DIR}" -G "${CMAKE_GENERATOR}" -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_C_COMPILER=${HOST_CC} -DCMAKE_CXX_COMPILER=${HOST_CXX}
        COMMAND ${CMAKE_COMMAND} --build "${ARKOI_TESTS_BUILD_DIR}"
        COMMAND ${CMAKE_CTEST_COMMAND} --test-dir "${ARKOI_TESTS_BUILD_DIR}" --output-on-failure
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ARKOI_TESTS_BUILD_DIR}/compile_commands.json" "${CMAKE_SOURCE_DIR}/tests/compile_commands.json"
        USES_TERMINAL
        COMMENT "Configure, build, and run hosted unit tests"
        VERBATIM
    )
    add_dependencies(arkoi_workspace host_tests)
endif()
