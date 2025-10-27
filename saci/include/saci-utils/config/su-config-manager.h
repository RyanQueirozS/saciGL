#ifndef SACI_UTILS_CONFIG_SU_CONFIG_MANAGER_H
#define SACI_UTILS_CONFIG_SU_CONFIG_MANAGER_H

#include <dylilo/dylilo.h>

#include "saci-utils/su-types-common.h"
#include "saci-utils/su-general.h"

enum su_RenderApi {
    su_RENDERER_API_OPENGL = 0,
    su_RENDERER_API_VULKAN,
};

enum su_RenderApiLoader {
    su_RENDERER_LOADER_GLAD = 0,
};

enum su_WindowApi {
    su_WINDOW_API_GLFW = 0,
};

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

struct su_RendererCfgUniform {
    char* name; // Debug
    su_DataType type;
    su_S32 location;
};

struct su_RendererCfgSampler {
    char* name; // Debug
    su_DataType type;
    su_S32 binding;
    su_U64 unit;
};

struct su_RendererCfgVertexLayout {
    char* name;
    su_DataType type;
    su_U64 offset;
    su_U32 location;
};

struct su_RendererCfgInstanceBufferLayout {
    char* name;
    su_DataType type;
    su_U64 offset;
    su_U32 location;
};

struct su_RendererCfgInstanceBuffer {
    char* name;
    su_U64 size_byte_internal;
    su_U64 layout_array_length;
    struct su_RendererCfgInstanceBufferLayout* layout_array;
};

struct su_RendererCfgBatch {
    su_U64 capacity;
    su_Bool fixed_capacity;

    struct {
        su_U64 capacity;
    } index_cfg;
    struct {
        su_U64 capacity;
    } vertex_cfg;
    struct {
        su_U64 capacity;
    } instance_cfg;
};

struct su_RendererCfgBound {
    struct {
        su_U64 capacity;
    } index_cfg;
    struct {
        su_U64 capacity;
    } instance_cfg;
};

enum su_RendererPrimitives {
    su_PRIMITIVES_TRIANGLES = 1,
};

enum su_RendererCullMode {
    su_RENDERER_CULL_MODE_BACK = 1,
};

enum su_RendererFrontFace {
    su_RENDERER_FRONT_FACE_CCW = 1,
};

struct su_RendererCfgDraw {
    enum su_RendererPrimitives primitive;
    enum su_RendererCullMode cull_mode;
    enum su_RendererFrontFace front_face;
};

struct su_RendererCfgBlend {
    su_Bool enabled;
    su_U64 opts;
};

struct su_RendererCfgPipeline {
    su_Bool depth_test;
    struct su_RendererCfgBlend blend;
};

struct su_RendererConfig {
    char* name; // Debug
    struct {
        char* frag;
        char* vert;
        char* geom;
    } shaders;
    su_U64 uniform_array_length;
    struct su_RendererCfgUniform* uniform_array;
    su_U64 sampler_array_length;
    struct su_RendererCfgSampler* sampler_array;

    struct su_RendererCfgVertexData {
        su_U64 element_size_internal;
        su_U64 layout_array_length;
        struct su_RendererCfgVertexLayout* layout_array;
    } vertex_data;

    struct {
        su_U64 element_size_internal;
    } index_data;

    struct {
        su_U64 buffer_array_length;
        struct su_RendererCfgInstanceBuffer* buffer_array;
    } instance_data;

    struct su_RendererCfgBatch batch;
    struct su_RendererCfgBound bound;
    struct su_RendererCfgDraw draw;
    struct su_RendererCfgPipeline pipeline;
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
