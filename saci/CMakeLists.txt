cmake_minimum_required(VERSION 3.10)

# Set the project name
project(saci LANGUAGES C)

# Set the output directories for the library
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable CMake to create the compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find all source files recursively in the src directory
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# Find all header files recursively in the include directory (optional)
file(GLOB_RECURSE HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.h"
)

# Find GLFW
find_package(glfw3 REQUIRED)

# Find GLEW
find_package(GLEW REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Create the library
add_library(saci STATIC ${SOURCES})

# Include directories
# Add the root include directory so that headers can be included as <directoryName/filename.h>
target_include_directories(saci PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"  # Root include directory
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/stbi" # Add your other dependencies here
    ${GLEW_INCLUDE_DIRS}   # Include GLEW headers
    ${GLFW_INCLUDE_DIRS}   # Include GLFW headers
)

# Link libraries
target_link_libraries(saci PUBLIC
    ${OPENGL_LIBRARIES}  # Link OpenGL
    glfw                 # Link GLFW
    GLEW::GLEW           # Link GLEW
)

# Add compiler flags
target_compile_options(saci PRIVATE
    -Wall         # Enable all warnings
    -Wextra       # Enable extra warnings
    -O2           # Optimization level
)

# Optionally, if you want different flags for different configurations
target_compile_options(saci PRIVATE
    $<$<CONFIG:Debug>:-g>  # Debugging information in Debug mode
    $<$<CONFIG:Release>:-O3> # Higher optimization in Release mode
)

# Optionally, if you have tests or examples, you can create separate executables
# Uncomment and modify these lines as needed
# add_executable(saci_example example.c)
# target_link_libraries(saci_example PRIVATE saci)

# Optionally, if you want to install the library
# install(TARGETS saci
#     EXPORT saciTargets
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
# )
# install(EXPORT saciTargets
#     FILE saciTargets.cmake
#     NAMESPACE saci::
#     DESTINATION lib/cmake/saci
# )
# install(FILES ${HEADERS} DESTINATION include/saci)

