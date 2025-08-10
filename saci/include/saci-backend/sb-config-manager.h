#ifndef SACI_BACKEND_SB_PLATFORM_H
#define SACI_BACKEND_SB_PLATFORM_H

#include <dylilo/dylilo.h>

#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"

typedef void (*sb_DebugCallback)(su_U32, su_U32, su_U32, su_U32, int, const char*, const void*);

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

    struct sb_WindowInitParams {
        int sdl_flags; // SDL specific
        int gl_major;
        int gl_minor;
        int profile;
    } init_window_params;

    struct sb_RenderApiFuncs {
        void (*clear_color)(float, float, float, float);
        void (*clear)(int);
        void (*enable)(int);
        void (*debug_message_callback)(sb_DebugCallback, void*);
        const su_U8* (*get_version_string)(void);
    } render_api_loader;

    struct sb_RenderApiLoaderFuncs {
        su_Bool (*load_opengl)(su_GLProc);
    } render_api_loader_funcs;

    struct sb_WindowingApiFuncs {
        int (*init)(void);
        void (*set_hint)(int, int);
        su_Window (*create_window)(int, int, const char*, su_Monitor, su_Window);
        su_GLProc (*get_proc)(void);
        void (*destroy_window)(su_Window);
        void (*make_context_current)(su_Window);
        su_Bool (*should_close)(su_Window);
        void (*set_pos_handler)(su_Window, su_WindowPosHandler);
        void (*set_size_handler)(su_Window, su_WindowSizeHandler);
        void (*terminate)(void);
        void (*swap_buffers)(su_Window);
        void (*poll_events)(void);
        void (*wait_events)(void);
        void (*wait_events_timeout)(double);
        void (*post_empty_event)(void);
        void (*set_mouse_pos_handler)(su_Window, su_EventMousePosHandler);
        su_Bool (*is_key_pressed)(su_Window, int);
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

SA_API struct sb_WindowingApiFuncs sb_cfg_manager_get_window_funcs(void);

#endif // SACI_BACKEND_SB_PLATFORM_H
