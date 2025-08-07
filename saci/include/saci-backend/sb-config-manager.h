#ifndef SACI_BACKEND_SB_PLATFORM_H
#define SACI_BACKEND_SB_PLATFORM_H

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
    const char* saci_lib_path;
    enum sb_RendererApi sc_renderer_api;
    enum sb_RenderApiLoader sc_render_api_loader;
    enum sb_WindowApi sc_window_api;
};

SA_INTERNAL_CONST struct sb_ConfigManager sb_CFG_MANAGER_DEFAULT = {
    .sc_renderer_api = sb_RENDERER_API_OPENGL,
    .sc_render_api_loader = sb_RENDER_API_LOADER_GLAD,
    .sc_window_api = sb_WINDOW_API_GLFW,
};

SA_API void sb_cfg_manager_load_default(void);

SA_API void sb_cfg_manager_set(const struct sb_ConfigManager cfg_manager);

SA_API void sb_cfg_manager_fetch(const char* path);

SA_API void sb_cfg_manager_load_dependencies(void);

SA_API enum sb_RendererApi sb_cfg_manager_get_renderer_api(void);

#endif // SACI_BACKEND_SB_PLATFORM_H
