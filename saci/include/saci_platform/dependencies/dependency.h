#ifndef SACI_BACKEND_RESOURCES_SB_DEPENDECY_H
#define SACI_BACKEND_RESOURCES_SB_DEPENDECY_H

#include "saci_util/defines.h"

#ifndef PSACI_DEFAULT_CONFIG_PATH
#  define PSACI_DEFAULT_CONFIG_PATH "saci.lua"
#endif // PSACI_DEFAULT_CONFIG_PATH

// Platform-specific default library paths
#if defined(_WIN32) && !defined(__EMSCRIPTEN__) // Windows
#  define PSACI_DEFAULT_OPENGL_PATH "C:\\Windows\\System32\\opengl32.dll"
#  define PSACI_DEFAULT_GLFW_PATH "C:\\Program Files\\GLFW\\lib\\glfw3.dll"
#  define PSACI_DEFAULT_GLAD_PATH "C:\\Program Files\\GLAD\\lib\\glad.dll"
#elif defined(__linux__) && !defined(__EMSCRIPTEN__) // Linux
#  define PSACI_DEFAULT_OPENGL_PATH "/usr/lib/x86_64-linux-gnu/libGL.so"
/* TODO NEEDS TO REMAIN LIKE THIS, REMOVE THE UNCOMENTED VERSION LATER
#  define PSACI_DEFAULT_GLFW_PATH "/usr/local/lib/libglfw.so"
#  define PSACI_DEFAULT_GLAD_PATH "/usr/local/lib/libglad.so"
*/
#  define PSACI_DEFAULT_GLFW_PATH "/lib/x86_64-linux-gnu/libglfw.so"
#  define PSACI_DEFAULT_GLAD_PATH "/home/ryan/.local/lib/glad/lib/libglad.so"
#elif defined(__APPLE__) && !defined(__EMSCRIPTEN__) // macOS
#  define PSACI_DEFAULT_OPENGL_PATH "/System/Library/Frameworks/OpenGL.framework/OpenGL"
#  define PSACI_DEFAULT_GLFW_PATH "/usr/local/lib/libglfw.dylib"
#  define PSACI_DEFAULT_GLAD_PATH "/usr/local/lib/libglad.dylib"
#elif defined(__EMSCRIPTEN__)
#  define PSACI_DEFAULT_OPENGL_PATH ""
#  define PSACI_DEFAULT_GLFW_PATH ""
#  define PSACI_DEFAULT_GLAD_PATH ""
#else
#  error "Unknown platform. Define paths for this platform."
#endif

enum PSaciRenderApi {
    PSACI_RENDERER_API_OPENGL4 = 1,
    PSACI_RENDERER_API_OPENGLES3 = 2,
    PSACI_RENDERER_API_VULKAN = 3,
};

enum PSaciRenderApiLoader {
    PSACI_RENDERER_LOADER_GLAD = 1,
    PSACI_RENDERER_LOADER_EMSCRIPTEN = 2,
};

enum PSaciWindowApi {
    PSACI_WINDOW_API_GLFW = 1,
    PSACI_WINDOW_API_EMSCRIPTEN = 2,
};

SACI_INTERNAL_CONST struct PSaciDependencyLoaderContext {
    char* cfg_file_path;

    struct {
        enum PSaciRenderApi api;
        char* path_to_api;
    } render_api_data;
    struct {
        enum PSaciRenderApiLoader api_loader;
        char* path_to_api;
    } render_api_loader_data;
    struct {
        enum PSaciWindowApi api;
        char* path_to_api;
    } windowing_api_data;
} PSACI_G_DEPENDENCY_CONTEXT_DEFAULT = {
    .cfg_file_path = PSACI_DEFAULT_CONFIG_PATH,
    .windowing_api_data = {
        .api = PSACI_WINDOW_API_GLFW,
        .path_to_api = PSACI_DEFAULT_GLFW_PATH,
    },
    .render_api_data = {
        .api = PSACI_RENDERER_API_OPENGL4,
        .path_to_api = PSACI_DEFAULT_OPENGL_PATH,
    },
    .render_api_loader_data = {
        .api_loader = PSACI_RENDERER_LOADER_GLAD,
        .path_to_api = PSACI_DEFAULT_GLAD_PATH,
    },
};

SACI_API void psaci_dependencies_load(const struct PSaciDependencyLoaderContext dependency_context);

SACI_API enum PSaciRenderApiLoader psaci_dependencies_get_render_loader(void);

SACI_API enum PSaciRenderApi psaci_dependencies_get_render_api(void);

SACI_API enum PSaciWindowApi psaci_dependencies_get_windowing_api(void);

#endif // SACI_BACKEND_RESOURCES_SB_DEPENDECY_H
