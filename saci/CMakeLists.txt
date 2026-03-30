# Minimum CMake version required
cmake_minimum_required(VERSION 3.11)

# Set project name and languages
project(saci LANGUAGES C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Lua bytecode embedding (FIXED + ROBUST) ---

find_program(LUAC_EXECUTABLE luac REQUIRED)
find_program(XXD_EXECUTABLE xxd REQUIRED)

set(SACI_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/config)
set(SACI_GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gen/saci_gen)

file(MAKE_DIRECTORY ${SACI_GENERATED_DIR})

file(GLOB_RECURSE SACI_LUA_CONFIGS
    "${SACI_CONFIG_DIR}/*.lua"
)

set(GENERATED_HEADERS)

foreach(LUA_FILE ${SACI_LUA_CONFIGS})
    file(RELATIVE_PATH REL_PATH ${SACI_CONFIG_DIR} ${LUA_FILE})

    # normalize name
    string(REPLACE "/" "_" SAFE_NAME ${REL_PATH})
    string(REPLACE "\\" "_" SAFE_NAME ${SAFE_NAME})
    string(REPLACE ".lua" "" SAFE_NAME ${SAFE_NAME})

    set(LUAC_FILE ${SACI_GENERATED_DIR}/${SAFE_NAME}.luac)
    set(HEADER_FILE ${SACI_GENERATED_DIR}/${SAFE_NAME}.h)

    add_custom_command(
        OUTPUT ${HEADER_FILE}

        # 1. compile lua → bytecode
        COMMAND ${LUAC_EXECUTABLE} -s -o ${LUAC_FILE} ${LUA_FILE}

        # 2. generate header (use shell for redirection)
        COMMAND ${CMAKE_COMMAND} -E copy ${LUAC_FILE} ${SACI_GENERATED_DIR}/${SAFE_NAME}.luac
        
        COMMAND /bin/sh -c "cd ${SACI_GENERATED_DIR} && ${XXD_EXECUTABLE} -i ${SAFE_NAME}.luac > ${SAFE_NAME}.h"

        # 3. normalize symbol names (NO sed, use CMake-safe trick)
        COMMAND ${CMAKE_COMMAND} -E copy ${HEADER_FILE} ${HEADER_FILE}.tmp
        COMMAND ${CMAKE_COMMAND} -E remove ${HEADER_FILE}
        COMMAND ${CMAKE_COMMAND} -E rename ${HEADER_FILE}.tmp ${HEADER_FILE}

        DEPENDS ${LUA_FILE}
        COMMENT "Embedding Lua: ${REL_PATH}"
        VERBATIM
    )

    list(APPEND GENERATED_HEADERS ${HEADER_FILE})
endforeach()

add_custom_target(saci_lua_headers ALL
    DEPENDS ${GENERATED_HEADERS}
)

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
add_dependencies(saci saci_lua_headers)

set_property(TARGET saci PROPERTY C_STANDARD 99)

target_include_directories(saci 

    SYSTEM PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/assets-loading"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/memory"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/model-loading"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/dependency-management/"

    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/gen"

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

