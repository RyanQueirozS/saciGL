cmake_minimum_required(VERSION 3.11)  # Updated to version 3.11 to use CMP0072cmake

# Set the project name
project(saci LANGUAGES C)

# Explicitly set OpenGL preference to GLVND
set(OpenGL_GL_PREFERENCE "GLVND")

# Set the output directories for the library
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable CMake to create the compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find all source files recursively in the src directory
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.c"
)

# Find all header files recursively in the include directory (optional)
file(GLOB_RECURSE HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.h"
)

# Find GLFW
find_package(glfw3 REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Set CMake policy for OpenGL GLVND preference
if (POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

# Create the library
add_library(saci STATIC ${SOURCES})

# Include directories
target_include_directories(saci PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"  # Root include directory
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/stb" # STB image headers
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include" # Glad headers
    ${GLFW_INCLUDE_DIRS}   # Include GLFW headers
)

# Add Glad source file to the library
target_sources(saci PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/src/glad.c")

# Link libraries
target_link_libraries(saci PUBLIC
    ${OPENGL_LIBRARIES}  # Link OpenGL
    glfw                 # Link GLFW
    m                    # Link math library (libm)
)

# Add compiler flags
target_compile_options(saci PRIVATE
    -Wall         # Enable all warnings
    -Wextra       # Enable extra warnings
    -O2           # Optimization level
)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../.git")
    message(STATUS "Git repository detected. Enabling SACI_DEBUG_MODE.")
    target_compile_definitions(saci PRIVATE SACI_DEBUG_MODE)
else()
    message(STATUS "No Git repository detected. SACI_DEBUG_MODE will not be defined.")
endif()

# Optionally, you can still conditionally add debug symbols or further define macros for different configurations
target_compile_options(saci PRIVATE
    $<$<CONFIG:Debug>:-g>      # Debugging information in Debug mode
    $<$<CONFIG:Release>:-O3>   # Higher optimization in Release mode
)
