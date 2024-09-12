# CMakeLists.txt for /examples/shapes_2d/

cmake_minimum_required(VERSION 3.10)

# Set policy CMP0072 to use GLVND (the modern OpenGL implementation)
cmake_policy(SET CMP0072 NEW)

# Project Name
project(shapes_2d)

# Set C standard
set(CMAKE_C_STANDARD 99)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLFW (version 3)
find_package(glfw3 REQUIRED)

# Find GLEW
find_package(GLEW REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/../../saci)
include_directories(${CMAKE_SOURCE_DIR}/../../deps/stbi)

# Add the executable
add_executable(shapes_2d shapes_2d.c)

# Link the necessary source files from saci
target_sources(
  shapes_2d
  PRIVATE ${CMAKE_SOURCE_DIR}/../../saci/core.c
          ${CMAKE_SOURCE_DIR}/../../saci/helpers.c
          ${CMAKE_SOURCE_DIR}/../../saci/scgl.c)

# Link libraries
target_link_libraries(shapes_2d OpenGL::GL glfw GLEW::GLEW)
