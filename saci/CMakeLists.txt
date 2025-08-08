# Minimum CMake version required
cmake_minimum_required(VERSION 3.11)

# Set project name and languages
project(saci LANGUAGES C)

set(OpenGL_GL_PREFERENCE "GLVND")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Emscripten check ---
set(IS_EMSCRIPTEN FALSE)
if("${CMAKE_C_COMPILER}" MATCHES "emcc")
    set(IS_EMSCRIPTEN TRUE)
    message(STATUS "Emscripten detected. Configuring for WebAssembly output.")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# --- External Dependencies ---
if(NOT IS_EMSCRIPTEN)
    set(GLFW_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/lib/GLFW/libglfw.so.3.4")
endif()

find_package(OpenGL REQUIRED)

if (POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

# --- Add Lua ---
add_subdirectory(deps/lua)

# --- Gather sources ---
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.c")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.h")

# --- Create SACI static library ---
add_library(saci STATIC ${SOURCES})

set_property(TARGET saci PROPERTY C_STANDARD 99)

target_include_directories(saci 
    SYSTEM PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/stb"
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/memory"
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/model-loading"
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/ryanqueirozs/"
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/lua/lua5.4"  # Headers only

    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"

    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_sources(saci PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/src/glad.c")
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/src/glad.c" PROPERTIES COMPILE_OPTIONS "-w")

# --- Link Libraries ---
if(IS_EMSCRIPTEN)
    target_link_libraries(saci PUBLIC
        lua
        ${OPENGL_LIBRARIES}
        m
        "-s USE_GLFW=3"
        "-s FULL_ES3=1"
        "-s ALLOW_MEMORY_GROWTH=1"
    )
else()
    target_link_libraries(saci PUBLIC
        lua
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARY}
        m
    )
    if(UNIX AND NOT APPLE)  # Linux only
        target_compile_definitions(saci PRIVATE _GNU_SOURCE)
        target_link_libraries(saci PUBLIC dl)
    endif()
endif()

# --- Compiler flags ---
set(SACI_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Path to SACI's dir")
target_compile_definitions(saci PRIVATE
    SACI_LIB_PATH="${SACI_LIB_PATH}"
)
target_compile_options(saci PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
    -Wunreachable-code
    -Wstrict-prototypes
    -Wmissing-declarations
    -Wunused-parameter
    -Wvla
    -Wunused-function
    -std=c99
    $<$<CONFIG:DEBUG>:-g>
    $<$<CONFIG:RELEASE>:-O3>
)

