cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0072 NEW)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(shapes_2d)

set(CMAKE_C_STANDARD 99)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

add_executable(shapes_2d shapes_2d.c)

target_include_directories(shapes_2d PRIVATE ${CMAKE_SOURCE_DIR}/../..)
target_include_directories(shapes_2d
                           PRIVATE ${CMAKE_SOURCE_DIR}/../../deps/stbi)

target_sources(
  shapes_2d
  PRIVATE ${CMAKE_SOURCE_DIR}/../../saci/core.c
          ${CMAKE_SOURCE_DIR}/../../saci/helpers.c
          ${CMAKE_SOURCE_DIR}/../../saci/scgl.c)

target_link_libraries(shapes_2d OpenGL::GL glfw GLEW::GLEW)
