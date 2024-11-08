# /tests/CMakeLists.txt

# Minimum CMake version and project setup
cmake_minimum_required(VERSION 3.10)
project(saci_tests)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add the saci library directory
add_subdirectory("${CMAKE_SOURCE_DIR}/../saci" "${CMAKE_BINARY_DIR}/saci_build")

# Collect the sources for the test executable
file(GLOB_RECURSE SACI_TEST_SOURCES "saci-core/*.c" "main.c")

file(GLOB_RECURSE DEPENDENCIES_SOURCES "saci-test/*.c")

# Include directories
include_directories(
    "${CMAKE_SOURCE_DIR}/saci-test/"
    "${CMAKE_SOURCE_DIR}/../saci/include"
)

# Define the test executable
add_executable(saci_tests ${SACI_TEST_SOURCES} ${DEPENDENCIES_SOURCES})

# Link the saci library to the test executable
target_link_libraries(saci_tests PRIVATE saci)

# Specify C standard and enable compiler warnings
set_property(TARGET saci_tests PROPERTY C_STANDARD 11)
target_compile_options(saci_tests PRIVATE -Wall -Wextra -pedantic)
