# Minimum CMake version required
cmake_minimum_required(VERSION 3.11)

# Set project name and languages
project(saci LANGUAGES C)

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

# --- Gather sources ---
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.c")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.h")

# --- Create SACI static library ---
add_library(saci STATIC ${SOURCES})

set_property(TARGET saci PROPERTY C_STANDARD 99)

target_include_directories(saci 
    SYSTEM PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/assets-loading"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/memory"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/model-loading"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/dependency-management/"

    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"

    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# --- Link Libraries ---
if(IS_EMSCRIPTEN)
    target_link_libraries(saci PUBLIC
        lua
        m
        "-s USE_GLFW=3"
        "-s FULL_ES3=1"
        "-s ALLOW_MEMORY_GROWTH=1"
    )
else()
    target_link_libraries(saci PUBLIC
        lua
        m
    )
    if(UNIX AND NOT APPLE)  # Linux only
        target_compile_definitions(saci PRIVATE _GNU_SOURCE)
        target_link_libraries(saci PUBLIC dl)
    endif()
endif()

# --- Compiler flags ---
set(SACI_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/saci" CACHE PATH "Path to SACI's dir")
set(SACI_PROEJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Path to SACI's dir")
target_compile_definitions(saci PRIVATE
    SACI_LIB_PATH="${SACI_LIB_PATH}"
    SACI_PROJECT_PATH="${SACI_PROJECT_PATH}"
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

