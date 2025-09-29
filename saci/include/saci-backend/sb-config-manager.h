#ifndef SACI_BACKEND_SB_PLATFORM_H
#define SACI_BACKEND_SB_PLATFORM_H

#include <dylilo/dylilo.h>

#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"

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
            void (*clear)(su_U32 mask);
            void (*enable)(su_U32 cap);
            void (*debug_message_callback)(void*, void*);
            const su_U8* (*get_string)(su_U32 name);

            void (*uniform1ui)(su_S32 location, su_U32 v0);
            void (*uniform1i)(su_S32 location, su_S32 v0);
            void (*uniform2f)(su_S32 location, float v0, float v1);
            void (*uniform3f)(su_S32 location, float v0, float v1, float v2);
            void (*uniform4f)(su_S32 location, float v0, float v1, float v2, float v3);

            void (*uniform_matrix_2fv)(su_S32 location, su_S32 count, unsigned char transpose, const float* value);
            void (*uniform_matrix_3fv)(su_S32 location, su_S32 count, unsigned char transpose, const float* value);
            void (*uniform_matrix_4fv)(su_S32 location, su_S32 count, unsigned char transpose, const float* value);
            void (*uniform_matrix_2x3fv)(su_S32 location, su_S32 count, unsigned char transpose, const float* value);
            void (*uniform_matrix_2x4fv)(su_S32 location, su_S32 count, unsigned char transpose, const float* value);
            void (*uniform_matrix_3x2fv)(su_S32 location, su_S32 count, unsigned char transpose, const float* value);
            void (*uniform_matrix_3x4fv)(su_S32 location, su_S32 count, unsigned char transpose, const float* value);
            void (*uniform_matrix_4x2fv)(su_S32 location, su_S32 count, unsigned char transpose, const float* value);
            void (*uniform_matrix_4x3fv)(su_S32 location, su_S32 count, unsigned char transpose, const float* value);

            void (*bind_vertex_array)(su_U32 array);
            void (*bind_buffer)(su_U32 target, su_U32 buffer);
            void (*buffer_data)(su_U32 target, long long size, const void* data, su_U32 usage);
            void (*buffer_subdata)(su_U32 target, long long offset, long long size, const void* data);
            void (*gen_buffers)(su_S32 n, su_U32* buffers);
            void (*gen_vertex_arrays)(su_S32 n, su_U32* arrays);
            void (*vertex_attrib_pointer)(su_U32 index, su_S32 size, su_U32 type, unsigned char normalized, su_S32 stride, const void* pointer);
            void (*vertex_attrib_divisor)(su_U32 index, su_U32 divisor);
            void (*enable_vertex_attrib_array)(su_U32 index);

            su_U32 (*create_shader)(su_U32 type);
            void (*shader_source)(su_U32 shader, su_S32 count, const char** string, const su_S32* length);
            void (*compile_shader)(su_U32 shader);
            void (*get_shaderiv)(su_U32 shader, su_U32 pname, su_S32* params);
            void (*get_shader_info_log)(su_U32 shader, su_S32 max_length, su_S32* length, char* info_log);
            void (*delete_shader)(su_U32 shader);

            void (*get_integer_v)(su_U32 pname, su_S32* params);
            void (*get_boolean_v)(su_U32 pname, unsigned char* data);

            void (*blend_func)(su_U32, su_U32);

            su_U32 (*get_error)(void);

            su_U32 (*create_program)(void);
            void (*use_program)(su_U32 program);
            void (*attach_shader)(su_U32 program, su_U32 shader);
            void (*link_program)(su_U32 program);
            void (*get_program_iv)(su_U32 program, su_U32 pname, su_S32* params);
            void (*get_program_info_log)(su_U32 program, su_S32 max_length, su_S32* length, char* info_log);
            void (*detach_shader)(su_U32 program, su_U32 shader);

            void (*draw_elements_instanced)(su_U32 mode, su_S32 count, su_U32 type, const void* indices, su_S32 instancecount);
            void (*draw_elements)(su_U32 mode, su_S32 count, su_U32 type, const void* indices);

            su_S32 (*get_uniform_location)(su_U32 program, const char* name);

            void (*gen_textures)(su_S32 n, su_U32* textures);
            void (*bind_texture)(su_U32 target, su_U32 texture);
            void (*active_texture)(su_U32 texture);
            void (*tex_image_2d)(su_U32 target, su_S32 level, su_S32 internalformat,
                                 su_S32 width, su_S32 height, su_S32 border,
                                 su_U32 format, su_U32 type, const void* data);
            void (*get_texlevel_parameter_iv)(su_U32 target, su_S32 level, su_U32 pname, su_S32* params);
            void (*generate_mipmap)(su_U32 target);
            void (*delete_textures)(su_S32 n, const su_U32* textures);
        } gl;
    } render_api_funcs;

    struct sb_RenderApiLoaderFuncs {
        struct {
            su_S32 (*load_opengl)(void* (*)(const char*));
        } gl;
    } render_api_loader_funcs;

    struct sb_WindowingApiFuncs {
        struct {
            int (*init)(void);
            void (*set_hint)(int, int);
            su_Window (*create_window)(int, int, const char*, su_Monitor, su_Window);
            void* (*get_proc)(const char*);
            void (*destroy_window)(su_Window);
            void (*make_context_current)(su_Window);
            su_S32 (*should_close)(su_Window);
            void (*set_pos_handler)(su_Window, void*);
            void (*set_size_handler)(su_Window, void*);
            void (*terminate)(void);
            void (*swap_buffers)(su_Window);
            void (*poll_events)(void);
            void (*wait_events)(void);
            void (*wait_events_timeout)(double);
            void (*post_empty_event)(void);
            void (*set_mouse_pos_handler)(su_Window, void*);
            su_S32 (*is_key_pressed)(su_Window, int);
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

struct sb_RendererCfgVertexLayout {
    su_String* name;
    su_DataType type;
    su_U64 offset;
    su_U32 location;
};

struct sb_RendererCfgInstanceBufferLayout {
    su_String* name;
    su_DataType type;
    su_U64 offset;
    su_U32 location;
};

struct sb_RendererCfgInstanceBuffer {
    su_String* name;
    su_S64 size_byte_internal;
    su_DArray* layout_array; // sb_RendererCfgInstanceBufferLayout
};

struct sb_RendererCfgBatch {
    su_U64 capacity;
    su_Bool fixed_capacity;

    struct {
        su_U64 capacity;
        su_Bool fixed_size;
    } index_cfg;
    struct {
        su_U64 capacity;
        su_Bool fixed_size;
    } vertex_cfg;
    struct {
        su_U64 capacity;
        su_Bool fixed_size;
    } instance_cfg;
};

struct sb_RendererCfgBound {
    struct {
        su_U64 capacity;
        su_Bool fixed_size;
    } index_cfg;
    struct {
        su_U64 capacity;
        su_Bool fixed_size;
    } instance_cfg;
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
    struct {
        su_String* frag;
        su_String* vert;
        su_String* geom;
    } shaders;
    su_DArray* uniform_array;
    su_DArray* sampler_array;

    struct {
        su_U64 element_size_internal;
        su_DArray* layout_array;
    } vertex_data;

    struct {
        su_U64 element_size_internal;
    } index_data;

    struct {
        su_DArray* buffer_array; // sb_RendererCfgInstanceBuffer
    } instance_data;

    struct sb_RendererCfgBatch batch;
    struct sb_RendererCfgBound bound;
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
