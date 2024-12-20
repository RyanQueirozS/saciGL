cmake_minimum_required(VERSION 3.11)

project(shapes_2d LANGUAGES C)

# Set CMake policies and configurations
cmake_policy(SET CMP0072 NEW)  # Use GLVND for OpenGL
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Specify the path to the saci library
set(SACI_DIR "${CMAKE_SOURCE_DIR}/../../../saci")

# Add the saci library as a subdirectory
add_subdirectory(${SACI_DIR} ${CMAKE_BINARY_DIR}/saci_build)

# Create the executable for this example
add_executable(shapes_2d shapes_2d_saci.c)

# Link libraries to the executable
target_link_libraries(shapes_2d PRIVATE
    saci              # Link the saci library
    glfw              # Link GLFW
    ${OPENGL_LIBRARIES}  # Link OpenGL
)

# Include directories from saci for the example
target_include_directories(shapes_2d PRIVATE
    ${SACI_DIR}/include  # Include saci's public headers
)

