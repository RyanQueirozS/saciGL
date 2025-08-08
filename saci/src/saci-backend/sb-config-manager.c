#define DYLILO_IMPL
#include "dylilo/dylilo.h"

#include "saci-backend/sb-config-manager.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"

// Platform-specific default library paths
#ifdef _WIN32 // Windows
#  define sb_DEFAULT_OPENGL_PATH "opengl32.dll"
#  define SB_DEFAULT_GLFW_PATH "glfw3.dll"
#  define SB_DEFAULT_GLAD_PATH "glad.dll"
#elif defined(__linux__) // Linux
#  define sb_DEFAULT_OPENGL_PATH "libGL.so"
#  define SB_DEFAULT_GLFW_PATH "libglfw.so"
#  define SB_DEFAULT_GLAD_PATH "libglad.so"
#elif defined(__APPLE__) // macOS
#  define sb_DEFAULT_OPENGL_PATH "libOpenGL.dylib"
#  define SB_DEFAULT_GLFW_PATH "libglfw.dylib"
#  define SB_DEFAULT_GLAD_PATH "libglad.dylib"
#else
#  error "Unknown platform. Define paths for this platform."
#endif

/* === Header impl === */

SA_INTERNAL struct sb_ConfigManager sb_cfg_manager = sb_CFG_MANAGER_DEFAULT;

void sb_cfg_manager_load_default(void) {
    sb_cfg_manager = sb_CFG_MANAGER_DEFAULT;
}

void sb_cfg_manager_set(const struct sb_ConfigManager cfg_manager) {
    sb_cfg_manager = cfg_manager;
}

void sb_cfg_manager_fetch(const char* path) {}

void sb_cfg_manager_load_dependencies(void) {
    switch (sb_cfg_manager.render_api_data.api) {
    case sb_RENDERER_API_OPENGL:
        dylilo_load_lib(sb_cfg_manager.render_api_data.path_to_api
                            ? sb_cfg_manager.render_api_data.path_to_api
                            : sb_DEFAULT_OPENGL_PATH,
                        DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_CONFIG,
                             "Invalid renderer api");
        exit(1);
    }

    switch (sb_cfg_manager.render_api_loader_data.api_loader) {
    case sb_RENDER_API_LOADER_GLAD:
        dylilo_load_lib(sb_cfg_manager.render_api_loader_data.path_to_api
                            ? sb_cfg_manager.render_api_loader_data.path_to_api
                            : SB_DEFAULT_GLAD_PATH,
                        DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_CONFIG,
                             "Invalid render api loader");
        exit(1);
    }

    switch (sb_cfg_manager.windowing_api_data.api) {
    case sb_WINDOW_API_GLFW:
        dylilo_load_lib(sb_cfg_manager.windowing_api_data.path_to_api
                            ? sb_cfg_manager.windowing_api_data.path_to_api
                            : SB_DEFAULT_GLFW_PATH,
                        DYLILO_FLAGS_DEFAULT);
        break;
    default:
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_CONFIG,
                             "Invalid windowing api");
        exit(1);
    }
}

enum sb_RendererApi sb_cfg_manager_get_renderer_api(void) {
    return sb_cfg_manager.render_api_data.api;
}
