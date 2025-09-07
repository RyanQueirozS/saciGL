#ifndef SACI_BACKEND_SB_PLATFORM_H
#define SACI_BACKEND_SB_PLATFORM_H

#include <dylilo/dylilo.h>

#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"

typedef void (*sb_DebugCallback)(su_U32, su_U32, su_U32, su_U32, int, const char*, const void*);

enum sb_RendererApi {
    sb_RENDERER_API_OPENGL = 0,
    sb_RENDERER_API_VULKAN,
};

enum sb_RenderApiLoader {
    sb_RENDERER_LOADER_GLAD = 0,
};

enum sb_WindowApi {
    sb_WINDOW_API_GLFW = 0,
};

struct sb_ConfigManager {
    su_String* cfg_file_path;

    struct {
        enum sb_RendererApi api;
        char* path_to_api;
        DyliloHandle handle;
    } render_api_data;
    struct {
        enum sb_RenderApiLoader api_loader;
        char* path_to_api;
        DyliloHandle handle;
    } render_api_loader_data;
    struct {
        enum sb_WindowApi api;
        char* path_to_api;
        DyliloHandle handle;
    } windowing_api_data;

    struct sb_WindowInitParams {
        int sdl_flags; // SDL specific
        int gl_major;
        int gl_minor;
        int profile;
    } init_window_params;

    struct sb_RenderApiFuncs {
        struct {
            void (*clear_color)(float, float, float, float);
            void (*clear)(int);
            void (*enable)(int);
            void (*debug_message_callback)(sb_DebugCallback, void*);
            const su_U8* (*get_version_string)(void);

            void (*uniform1ui)(int, su_U32);
            void (*uniform1i)(int, int);
            void (*uniform2f)(int, float, float);
            void (*uniform3f)(int, float, float, float);
            void (*uniform4f)(int, float, float, float, float);
            void (*uniform_matrix_2fv)(int, su_U64, su_Bool, const float*);
            void (*uniform_matrix_3fv)(int, su_U64, su_Bool, const float*);
            void (*uniform_matrix_4fv)(int, su_U64, su_Bool, const float*);
            void (*uniform_matrix_2x3fv)(int, su_U64, su_Bool, const float*);
            void (*uniform_matrix_2x4fv)(int, su_U64, su_Bool, const float*);
            void (*uniform_matrix_3x2fv)(int, su_U64, su_Bool, const float*);
            void (*uniform_matrix_3x4fv)(int, su_U64, su_Bool, const float*);
            void (*uniform_matrix_4x2fv)(int, su_U64, su_Bool, const float*);
            void (*uniform_matrix_4x3fv)(int, su_U64, su_Bool, const float*);

            void (*bind_vertex_array)(uint);
            void (*bind_buffer)(int, uint);
            void (*buffer_data)(int, su_U64, const void*, su_U32);
            void (*buffer_subdata)(int, su_U64, su_U32, const void*);
            void (*gen_buffers)(su_U64, su_U32*);
            void (*gen_vertex_arrays)(su_U64, su_U32*);
            void (*vertex_attrib_pointer)(su_U32, int, su_U32, su_Bool, su_U64, const void*);
            void (*vertex_attrib_divisor)(su_U32, su_U32);
            void (*enable_vertex_attrib_array)(su_U32);

            su_U32 (*create_shader)(su_U32);
            void (*shader_source)(su_U32, su_U64, const char* const*, const int*);
            void (*compile_shader)(su_U32);
            void (*get_shaderiv)(su_U32, int, int*);
            void (*get_shader_info_log)(su_U32, su_U64, int*, char*);
            void (*delete_shader)(su_U32);

            su_U32 (*create_program)(void);
            void (*use_program)(su_U32);
            void (*attach_shader)(su_U32, su_U32);
            void (*link_program)(su_U32);
            void (*get_programiv)(su_U32, int, int*);
            void (*get_program_info_log)(su_U32, su_U64, int*, char*);
            void (*detach_shader)(su_U32, su_U32);

            void (*draw_elements_instanced)(int, int, int, int, int);
            void (*draw_elements)(su_U32 mode, su_U32 count, su_U32 type, const void* indices);

            int (*get_uniform_location)(su_U32, const char*);

            void (*gen_textures)(su_U64, su_U32*);
            void (*bind_texture)(su_U32, su_U32);
            void (*active_texture)(su_U32 texture);
            void (*tex_image_2d)(su_U32, int, int, int, int, int, su_U32, int, const void*);
            void (*get_texlevel_parameter_iv)(su_U32, int, su_U32, int*);
            void (*generate_mipmap)(su_U32);
            void (*delete_textures)(su_U64, const su_U32*);
        } gl;
    } render_api_funcs;

    struct sb_RenderApiLoaderFuncs {
        struct {
            su_Bool (*load_opengl)(su_GLProc);
        } gl;
    } render_api_loader_funcs;

    struct sb_WindowingApiFuncs {
        struct {
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
        } glfw;
    } window_funcs;
};

SA_INTERNAL_CONST struct sb_ConfigManager sb_CFG_MANAGER_DEFAULT = {
    .render_api_data = {
        .api = sb_RENDERER_API_OPENGL,
        .path_to_api = NULL,
    },
    .render_api_loader_data = {
        .api_loader = sb_RENDERER_LOADER_GLAD,
        .path_to_api = NULL,
    },
    .windowing_api_data = {
        .api = sb_WINDOW_API_GLFW,
        .path_to_api = NULL,
    },
};

struct sb_RendererCfgShaderCode {
    su_String* frag;
    su_String* vert;
    su_String* geom;
};

struct sb_RendererCfgUniform {
    su_String* name; // Debug
    su_DataType type;
    su_S32 location;
};

struct sb_RendererCfgSampler {
    su_String* name; // Debug
    su_DataType type;
    su_S32 binding;
    su_U64 unit;
};

struct sb_RendererCfgIndex {
    su_U64 capacity;
    su_Bool fixed_size;
    su_U64 element_size;
};

struct sb_RendererCfgVertexLayout {
    su_String* name;
    su_DataType type;
    su_U64 offset;
    su_U64 location;
};

struct sb_RendererCfgVertex {
    su_U64 capacity;
    su_Bool fixed_size;
    su_U64 struct_size;
    su_DArray* vertex_layout_array;
};

struct sb_RendererCfgInstanceBufferLayout {
    su_String* name;
    su_DataType type;
    su_U64 offset;
    su_U64 location;
};

struct sb_RendererCfgInstanceBuffer {
    su_String* name;
    su_DArray* layout_array;
};

struct sb_RendererCfgInstance {
    su_U64 capacity;
    su_Bool fixed_size;
    su_DArray* buffer_array;
};

struct sb_RendererCfgBatch {
    su_U64 capacity;
    su_Bool fixed_capacity;

    struct sb_RendererCfgIndex index_cfg;
    struct sb_RendererCfgVertex vertex_cfg;
    struct sb_RendererCfgInstance instance_cfg;
};

enum sb_RendererPrimitives {
    sb_PRIMITIVES_TRIANGLES = 1,
};

enum sb_RendererCullMode {
    sb_RENDERER_CULL_MODE_BACK = 1,
};

enum sb_RendererFrontFace {
    sb_RENDERER_FRONT_FACE_CCW = 1,
};

struct sb_RendererCfgDraw {
    enum sb_RendererPrimitives primitive;
    enum sb_RendererCullMode cull_mode;
    enum sb_RendererFrontFace front_face;
};

struct sb_RendererCfgBlend {
    su_Bool enabled;
    su_U64 opts;
};

struct sb_RendererCfgPipeline {
    su_Bool depth_test;
    struct sb_RendererCfgBlend blend;
};

struct sb_RendererConfig {
    su_String* name; // Debug
    struct sb_RendererCfgShaderCode shaders;
    su_DArray* uniform_array;
    su_DArray* sampler_array;
    struct sb_RendererCfgBatch batch;
    struct sb_RendererCfgDraw draw;
    struct sb_RendererCfgPipeline pipeline;
};

SA_API void sb_cfg_manager_load_default(void);

SA_API void sb_cfg_manager_set(const struct sb_ConfigManager cfg_manager);

SA_API su_Bool sb_cfg_manager_fetch(const char* path);

SA_API void sb_cfg_manager_load_dependencies(void);

SA_API enum sb_RendererApi sb_cfg_manager_get_renderer_api(void);

SA_API struct sb_WindowingApiFuncs sb_cfg_manager_get_window_funcs(void);

SA_API struct sb_RenderApiLoaderFuncs sb_cfg_manager_get_loader_funcs(void);

SA_API struct sb_RenderApiFuncs sb_cfg_manager_get_render_funcs(void);

SA_API void sb_cfg_manager_get_renderer(su_String* name, struct sb_RendererConfig* cfg_out);

#endif // SACI_BACKEND_SB_PLATFORM_H
