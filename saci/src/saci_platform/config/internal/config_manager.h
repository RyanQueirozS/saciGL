#ifndef SACI_UTILS_CONFIG_SU_CONFIG_MANAGER_H
#define SACI_UTILS_CONFIG_SU_CONFIG_MANAGER_H

#include <dylilo/dylilo.h>

#include "saci_util/types.h"
#include "saci_util/defines.h"

#include "saci_platform/config/config.h"

struct PSaciConfigManager {
    char* cfg_file_path;

    struct {
        enum PSaciRenderApi api;
        char* path_to_api;
        DyliloHandle handle;
    } render_api_data;
    struct {
        enum PSaciRenderApiLoader api_loader;
        char* path_to_api;
        DyliloHandle handle;
    } render_api_loader_data;
    struct {
        enum PSaciWindowApi api;
        char* path_to_api;
        DyliloHandle handle;
    } windowing_api_data;

    struct PSaciWindowInitParams {
        int sdl_flags; // SDL specific
        int gl_major;
        int gl_minor;
        int profile;
    } init_window_params;
};

SACI_INTERNAL_CONST struct PSaciConfigManager PSACI_G_CFG_MANAGER_DEFAULT = {
    .render_api_data = {
        .api = PSACI_RENDERER_API_OPENGL,
        .path_to_api = NULL,
    },
    .render_api_loader_data = {
        .api_loader = PSACI_RENDERER_LOADER_GLAD,
        .path_to_api = NULL,
    },
    .windowing_api_data = {
        .api = PSACI_WINDOW_API_GLFW,
        .path_to_api = NULL,
    },
};

SACI_API void psaci_cfg_manager_load_default(void);

SACI_API void psaci_cfg_manager_set(const struct PSaciConfigManager cfg_manager);

SACI_API SaciBool psaci_cfg_manager_fetch(const char* path);

SACI_API enum PSaciRenderApi psaci_cfg_manager_get_renderer_api(void);

const char* psaci_cfg_manager_get_renderer_api_path(void);

SACI_API enum PSaciRenderApiLoader psaci_cfg_manager_get_renderer_api_loader(void);

const char* psaci_cfg_manager_get_renderer_api_loader_path(void);

SACI_API enum PSaciWindowApi psaci_cfg_manager_get_window_api(void);

const char* psaci_cfg_manager_get_window_api_path(void);

SACI_API void psaci_cfg_manager_get_renderer(const char* name, struct PSaciRendererConfig* cfg_out);

SACI_API SaciU64 psaci_cfg_manager_render_cfg_size(const struct PSaciRendererConfig* cfg);

SACI_API void psaci_cfg_manager_cleanup_renderer_cfg(struct PSaciRendererConfig* cfg);

#endif // SACI_UTILS_CONFIG_SU_CONFIG_MANAGER_H
