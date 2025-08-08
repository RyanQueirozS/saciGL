#ifndef SACI_BACKEND_SB_PLATFORM_H
#define SACI_BACKEND_SB_PLATFORM_H

#include <dylilo/dylilo.h>

#include <saci-utils/su-general.h>

enum sb_RendererApi {
    sb_RENDERER_API_OPENGL = 0,
};

enum sb_RenderApiLoader {
    sb_RENDER_API_LOADER_GLAD = 0,
};

enum sb_WindowApi {
    sb_WINDOW_API_GLFW = 0,
};

struct sb_ConfigManager {
    struct {
        enum sb_RendererApi api;
        char* path_to_api;
    } render_api_data;
    struct {
        enum sb_RenderApiLoader api_loader;
        char* path_to_api;
    } render_api_loader_data;
    struct {
        enum sb_WindowApi api;
        char* path_to_api;
    } windowing_api_data;

    struct {
        DyliloHandle handle;
        void* init_func;
        void* load_proc_func;
    } window_funcs;
};

SA_INTERNAL_CONST struct sb_ConfigManager sb_CFG_MANAGER_DEFAULT = {
    .render_api_data = {
        .api = sb_RENDERER_API_OPENGL,
        .path_to_api = NULL,
    },
    .render_api_loader_data = {
        .api_loader = sb_RENDER_API_LOADER_GLAD,
        .path_to_api = NULL,
    },
    .windowing_api_data = {
        .api = sb_WINDOW_API_GLFW,
        .path_to_api = NULL,
    },
};

SA_API void sb_cfg_manager_load_default(void);

SA_API void sb_cfg_manager_set(const struct sb_ConfigManager cfg_manager);

SA_API void sb_cfg_manager_fetch(const char* path);

SA_API void sb_cfg_manager_load_dependencies(void);

SA_API enum sb_RendererApi sb_cfg_manager_get_renderer_api(void);

#endif // SACI_BACKEND_SB_PLATFORM_H
