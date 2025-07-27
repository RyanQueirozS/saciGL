#if 0
#  ifndef SACI_BACKEND_SB_PLATFORM_H
#    define SACI_BACKEND_SB_PLATFORM_H

enum sb_RendererApi {
    sb_RENDERER_API_OPENGL = 0,
};

enum sb_RenderApiLoader {
    sb_RENDER_API_LOADER_GLAD = 0,
};

enum sb_WindowApi {
    sb_WINDOW_API_GLFW = 0,
};

enum sb_RendererApi sb_cfg_manager_get_renderer_api(void);

#  endif // SACI_BACKEND_SB_PLATFORM_H
#endif
