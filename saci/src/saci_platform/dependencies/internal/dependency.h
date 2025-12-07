#ifndef SACI_BACKEND_RESOURCES_SB_DEPENDENCY_INTERNAL_H
#define SACI_BACKEND_RESOURCES_SB_DEPENDENCY_INTERNAL_H

#include "saci_util/types.h"
#include "saci_util/defines.h"

struct PSaciWindowingApiFuncs {
    struct {
        int (*init)(void);
        void (*set_hint)(int, int);
        void* (*create_window)(int, int, const char*, void*, void*);
        void* (*get_proc)(const char*);
        void (*destroy_window)(void*);
        void (*make_context_current)(void*);
        SaciS32 (*should_close)(void*);
        void (*set_pos_handler)(void*, void*);
        void (*set_size_handler)(void*, void*);
        void (*terminate)(void);
        void (*swap_buffers)(void*);
        void (*poll_events)(void);
        void (*wait_events)(void);
        void (*wait_events_timeout)(double);
        void (*post_empty_event)(void);
        void (*set_mouse_pos_handler)(void*, void*);
        SaciS32 (*is_key_pressed)(void*, int);
    } glfw;
};

struct PSaciRenderApiFuncs {
    struct {
        void (*clear_color)(float, float, float, float);
        void (*clear)(SaciU32 mask);
        void (*enable)(SaciU32 cap);
        void (*debug_message_callback)(void*, void*);
        const SaciU8* (*get_string)(SaciU32 name);

        void (*uniform1ui)(SaciS32 location, SaciU32 v0);
        void (*uniform1i)(SaciS32 location, SaciS32 v0);
        void (*uniform2f)(SaciS32 location, float v0, float v1);
        void (*uniform3f)(SaciS32 location, float v0, float v1, float v2);
        void (*uniform4f)(SaciS32 location, float v0, float v1, float v2, float v3);

        void (*uniform_matrix_2fv)(SaciS32 location, SaciS32 count, unsigned char transpose, const float* value);
        void (*uniform_matrix_3fv)(SaciS32 location, SaciS32 count, unsigned char transpose, const float* value);
        void (*uniform_matrix_4fv)(SaciS32 location, SaciS32 count, unsigned char transpose, const float* value);
        void (*uniform_matrix_2x3fv)(SaciS32 location, SaciS32 count, unsigned char transpose, const float* value);
        void (*uniform_matrix_2x4fv)(SaciS32 location, SaciS32 count, unsigned char transpose, const float* value);
        void (*uniform_matrix_3x2fv)(SaciS32 location, SaciS32 count, unsigned char transpose, const float* value);
        void (*uniform_matrix_3x4fv)(SaciS32 location, SaciS32 count, unsigned char transpose, const float* value);
        void (*uniform_matrix_4x2fv)(SaciS32 location, SaciS32 count, unsigned char transpose, const float* value);
        void (*uniform_matrix_4x3fv)(SaciS32 location, SaciS32 count, unsigned char transpose, const float* value);

        void (*bind_vertex_array)(SaciU32 array);
        void (*bind_buffer)(SaciU32 target, SaciU32 buffer);
        void (*buffer_data)(SaciU32 target, long long size, const void* data, SaciU32 usage);
        void (*buffer_subdata)(SaciU32 target, long long offset, long long size, const void* data);
        void (*gen_buffers)(SaciS32 n, SaciU32* buffers);
        void (*gen_vertex_arrays)(SaciS32 n, SaciU32* arrays);
        void (*vertex_attrib_pointer)(SaciU32 index, SaciS32 size, SaciU32 type, unsigned char normalized, SaciS32 stride, const void* pointer);
        void (*vertex_attrib_divisor)(SaciU32 index, SaciU32 divisor);
        void (*enable_vertex_attrib_array)(SaciU32 index);

        SaciU32 (*create_shader)(SaciU32 type);
        void (*shader_source)(SaciU32 shader, SaciS32 count, const char** string, const SaciS32* length);
        void (*compile_shader)(SaciU32 shader);
        void (*get_shaderiv)(SaciU32 shader, SaciU32 pname, SaciS32* params);
        void (*get_shader_info_log)(SaciU32 shader, SaciS32 max_length, SaciS32* length, char* info_log);
        void (*delete_shader)(SaciU32 shader);

        void (*get_integer_v)(SaciU32 pname, SaciS32* params);
        void (*get_boolean_v)(SaciU32 pname, unsigned char* data);

        void (*blend_func)(SaciU32, SaciU32);

        SaciU32 (*get_error)(void);

        SaciU32 (*create_program)(void);
        void (*use_program)(SaciU32 program);
        void (*attach_shader)(SaciU32 program, SaciU32 shader);
        void (*link_program)(SaciU32 program);
        void (*get_program_iv)(SaciU32 program, SaciU32 pname, SaciS32* params);
        void (*get_program_info_log)(SaciU32 program, SaciS32 max_length, SaciS32* length, char* info_log);
        void (*detach_shader)(SaciU32 program, SaciU32 shader);

        void (*draw_elements_instanced)(SaciU32 mode, SaciS32 count, SaciU32 type, const void* indices, SaciS32 instancecount);
        void (*draw_elements)(SaciU32 mode, SaciS32 count, SaciU32 type, const void* indices);

        SaciS32 (*get_uniform_location)(SaciU32 program, const char* name);

        void (*gen_textures)(SaciS32 n, SaciU32* textures);
        void (*bind_texture)(SaciU32 target, SaciU32 texture);
        void (*active_texture)(SaciU32 texture);
        void (*tex_image_2d)(SaciU32 target, SaciS32 level, SaciS32 internalformat,
                             SaciS32 width, SaciS32 height, SaciS32 border,
                             SaciU32 format, SaciU32 type, const void* data);
        void (*get_texlevel_parameter_iv)(SaciU32 target, SaciS32 level, SaciU32 pname, SaciS32* params);
        void (*generate_mipmap)(SaciU32 target);
        void (*delete_textures)(SaciS32 n, const SaciU32* textures);
    } gl;
};

struct PSaciRenderApiLoaderFuncs {
    struct {
        SaciS32 (*load_opengl)(void* (*)(const char*));
    } gl;
};

SACI_API struct PSaciWindowingApiFuncs psaci_dependencies_get_windowing_api_funcs(void);

SACI_API struct PSaciRenderApiFuncs psaci_dependencies_get_render_api_funcs(void);

SACI_API struct PSaciRenderApiLoaderFuncs psaci_dependencies_get_render_loader_api_funcs(void);

#endif // SACI_BACKEND_RESOURCES_SB_DEPENDENCY_INTERNAL_H
