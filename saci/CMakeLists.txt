# Minimum CMake version required
cmake_minimum_required(VERSION 3.11)  # Version 3.11 to use CMP0072

# Set project name and languages
project(saci LANGUAGES C)

# Set OpenGL preference to GLVND
set(OpenGL_GL_PREFERENCE "GLVND")

# Set output directories for the library and binaries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable CMake to create compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find all source files recursively in the src directory
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.c")

# Find all header files recursively in the include directory
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.h")

# --- Find Dependencies ---

# Finding glfw
set(GLFW_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/lib/GLFW/libglfw.so.3.4")

# Find OpenGL
find_package(OpenGL REQUIRED)

# Set CMake policy for OpenGL GLVND preference
if (POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

# --- Library Setup ---

# Create the static library
add_library(saci STATIC ${SOURCES})

# Set C standard to C99
set_property(TARGET saci PROPERTY C_STANDARD 99)

# Include directories
target_include_directories(saci SYSTEM PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/stb"           # STB image headers
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include"  # Glad headers
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/model-loading" # Model Loaders headers
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include"  # GLFW headers

    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"            # Root include directory
)

# Add Glad source file to the library
target_sources(saci PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/src/glad.c")
# Disable warnings for glad.c
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/src/glad.c" PROPERTIES COMPILE_OPTIONS "-w")

# --- Link Libraries ---
target_link_libraries(saci PUBLIC
    ${OPENGL_LIBRARIES}  # Link OpenGL
    ${GLFW_LIBRARY}      # Link GLFW
    m                    # Link math library (libm)
)

# --- Debug Mode ---

# Check if the project is in a Git repository
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../.git")
    message(STATUS "Git repository detected. Enabling SACI_DEBUG_MODE.")
    target_compile_definitions(saci PRIVATE SACI_DEBUG_MODE)
else()
    message(STATUS "No Git repository detected. SACI_DEBUG_MODE will not be defined.")
endif()

# --- Compiler Flags ---
# Default compiler flags for all source files in saci (except glad.c)
target_compile_options(saci PRIVATE
    -Wall                      # Enable all warnings
    -Wextra                    # Enable extra warnings
    -Wpedantic                 # Enable pedantic warnings
    -Wshadow                   # Warn when a local variable or function shadows another
    -Wconversion               # Warn about implicit type conversions that may lose data
    -Wunreachable-code         # Warn about code that will never be executed
    -Wstrict-prototypes        # Warn if a function declaration doesn't include argument types
    -Wmissing-declarations     # Warn when a function is used but not declared
    -Wunused-parameter         # Ignore unused parameter warnings
    -Wvla                      # Warn if variable-length arrays (VLAs) are used (C99 feature)
    -Wunused-function          # Avoid warning for unused static functions
    -std=c99                   # Enforce C99 standard

    # Debug and release-specific flags
    $<$<CONFIG:DEBUG>:-g>       # Debugging information for debug mode
    $<$<CONFIG:RELEASE>:-O3>    # Higher optimization in release mode
)

# --- Clang-specific settings ---
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(STATUS "Clang compiler detected")

    # Add Clang-specific compile options
    target_compile_options(saci PRIVATE
    )
endif()
