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
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.h")

# --- Detect Emscripten ---
set(IS_EMSCRIPTEN FALSE)
if("${CMAKE_C_COMPILER}" MATCHES "emcc")
    set(IS_EMSCRIPTEN TRUE)
    message(STATUS "Emscripten detected. Configuring for WebAssembly output.")

    # WebAssembly-specific compiler and linker flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1")

    # Produce .html files by default
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# --- Find Dependencies ---

# GLFW (only link native .so if not Emscripten)
if(NOT IS_EMSCRIPTEN)
    set(GLFW_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/lib/GLFW/libglfw.so.3.4")
endif()

# OpenGL is always required
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
target_include_directories(saci 
    SYSTEM PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/stb"           # STB image headers
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/memory"        # Memory headers
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include"  # Glad headers
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/model-loading" # Model Loaders headers
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include"  # GLFW headers
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include"  # GLFW headers
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/lua/"  # Lua headers

    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/"               # Internal includes

    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"            # Public headers
)

# Add Glad source file to the library
target_sources(saci PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/src/glad.c")
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/src/glad.c" PROPERTIES COMPILE_OPTIONS "-w")

# Add Lua source file to the library
file(GLOB LUA_SRC "${CMAKE_CURRENT_SOURCE_DIR}/deps/lua/lua5.4/*.c")
target_sources(saci PRIVATE ${LUA_SRC})
set_source_files_properties(${LUA_SRC} PROPERTIES COMPILE_OPTIONS "-w")

# --- Link Libraries ---
if(IS_EMSCRIPTEN)
    target_link_libraries(saci PUBLIC
        ${OPENGL_LIBRARIES}
        m
        "-s USE_GLFW=3"
        "-s FULL_ES3=1"
        "-s ALLOW_MEMORY_GROWTH=1"
    )
else()
    target_link_libraries(saci PUBLIC
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARY}
        m
    )
endif()

# --- Debug Mode ---
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../.git")
    message(STATUS "Git repository detected. Enabling SACI_DEBUG_MODE.")
    target_compile_definitions(saci PRIVATE SACI_DEBUG_MODE_ALL)
else()
    message(STATUS "No Git repository detected. SACI_DEBUG_MODE will not be defined.")
endif()

# --- Compiler Flags ---
target_compile_options(saci PRIVATE
    -Wall
    -Wextra
    # -Werror
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

# --- Clang-specific settings ---
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(STATUS "Clang compiler detected")
endif()
