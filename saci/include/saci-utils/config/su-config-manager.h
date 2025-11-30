#ifndef SACI_UTILS_CONFIG_SU_CONFIG_MANAGER_H
#define SACI_UTILS_CONFIG_SU_CONFIG_MANAGER_H

#include <dylilo/dylilo.h>

#include "saci-utils/su-types-common.h"
#include "saci-utils/su-general.h"

#include "./su-config.h"

struct su_ConfigManager {
    char* cfg_file_path;

    struct {
        enum su_RenderApi api;
        char* path_to_api;
        DyliloHandle handle;
    } render_api_data;
    struct {
        enum su_RenderApiLoader api_loader;
        char* path_to_api;
        DyliloHandle handle;
    } render_api_loader_data;
    struct {
        enum su_WindowApi api;
        char* path_to_api;
        DyliloHandle handle;
    } windowing_api_data;

    struct su_WindowInitParams {
        int sdl_flags; // SDL specific
        int gl_major;
        int gl_minor;
        int profile;
    } init_window_params;
};

SA_INTERNAL_CONST struct su_ConfigManager su_CFG_MANAGER_DEFAULT = {
    .render_api_data = {
        .api = su_RENDERER_API_OPENGL,
        .path_to_api = NULL,
    },
    .render_api_loader_data = {
        .api_loader = su_RENDERER_LOADER_GLAD,
        .path_to_api = NULL,
    },
    .windowing_api_data = {
        .api = su_WINDOW_API_GLFW,
        .path_to_api = NULL,
    },
};

SA_API void su_cfg_manager_load_default(void);

SA_API void su_cfg_manager_set(const struct su_ConfigManager cfg_manager);

SA_API su_Bool su_cfg_manager_fetch(const char* path);

SA_API enum su_RenderApi su_cfg_manager_get_renderer_api(void);

const char* su_cfg_manager_get_renderer_api_path(void);

SA_API enum su_RenderApiLoader su_cfg_manager_get_renderer_api_loader(void);

const char* su_cfg_manager_get_renderer_api_loader_path(void);

SA_API enum su_WindowApi su_cfg_manager_get_window_api(void);

const char* su_cfg_manager_get_window_api_path(void);

SA_API void su_cfg_manager_get_renderer(const char* name, struct su_RendererConfig* cfg_out);

SA_API su_U64 su_cfg_manager_render_cfg_size(const struct su_RendererConfig* cfg);

SA_API void su_cfg_manager_cleanup_renderer_cfg(struct su_RendererConfig* cfg);

#endif // SACI_UTILS_CONFIG_SU_CONFIG_MANAGER_H
