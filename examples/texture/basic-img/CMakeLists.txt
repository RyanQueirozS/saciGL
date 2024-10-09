cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0072 NEW)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(basic-img LANGUAGES C)

set(CMAKE_C_STANDARD 99)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Specify the path to the saci library
set(SACI_DIR "${CMAKE_SOURCE_DIR}/../../../saci")

# Add the saci library as a subdirectory
add_subdirectory(${SACI_DIR} ${CMAKE_BINARY_DIR}/saci_build)

# Include directories for the saci library
include_directories(${SACI_DIR})

# Create the executable
add_executable(basic-img basic-img_texture_saci.c)

# Link libraries
target_link_libraries(basic-img PRIVATE 
    saci
)

