cmake_minimum_required(VERSION 3.10)
project(saci)

# Set the OpenGL preference to GLVND
set(OpenGL_GL_PREFERENCE "GLVND")

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Find GLFW3
find_package(glfw3 3.0 REQUIRED)
# Find GLEW
# TODO set glew version
find_package(GLEW REQUIRED)
# Find OpenGL
find_package(OpenGL REQUIRED)

# Globbing source files
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/saci/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/**/*.c"
)

# Globbing header files
file(GLOB_RECURSE HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/saci/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/**/*.h"
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} examples/composition2d.c)

# Specify compiler options for GCC
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
endif()

add_compile_options(-O0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps)
include_directories(${GLEW_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
