#ifndef SACI_BACKEND_SB_GL_H
#define SACI_BACKEND_SB_GL_H

#include "saci_util/defines.h"
#include "saci_util/types.h"

enum PSaciGLConstants {
    PSACI_GL_TRIANGLES = 0x0004,
    PSACI_GL_DEPTH_TEST = 0x0B71,
    PSACI_GL_TEXTURE_2D = 0x0DE1,
    PSACI_GL_TEXTURE_WIDTH = 0x1000,
    PSACI_GL_TEXTURE_HEIGHT = 0x1001,
    PSACI_GL_UNSIGNED_BYTE = 0x1401,
    PSACI_GL_UNSIGNED_INT = 0x1405,
    PSACI_GL_FLOAT = 0x1406,
    PSACI_GL_TEXTURE_BINDING_2D = 0x8069,
    PSACI_GL_DEBUG_OUTPUT_SYNCHRONOUS = 0x8242,
    PSACI_GL_DEBUG_SOURCE_API = 0x8246,
    PSACI_GL_DEBUG_SOURCE_WINDOW_SYSTEM = 0x8247,
    PSACI_GL_DEBUG_SOURCE_SHADER_COMPILER = 0x8248,
    PSACI_GL_DEBUG_SOURCE_THIRD_PARTY = 0x8249,
    PSACI_GL_DEBUG_SOURCE_APPLICATION = 0x824A,
    PSACI_GL_DEBUG_SOURCE_OTHER = 0x824B,
    PSACI_GL_DEBUG_TYPE_ERROR = 0x824C,
    PSACI_GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR = 0x824D,
    PSACI_GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR = 0x824E,
    PSACI_GL_DEBUG_TYPE_PORTABILITY = 0x824F,
    PSACI_GL_DEBUG_TYPE_PERFORMANCE = 0x8250,
    PSACI_GL_DEBUG_TYPE_OTHER = 0x8251,
    PSACI_GL_DEBUG_TYPE_MARKER = 0x8268,
    PSACI_GL_UNSIGNED_SHORT = 0x1403,
    PSACI_GL_BYTE = 0x1400,
    PSACI_GL_SHORT = 0x1402,
    PSACI_GL_INT = 0x1404,
    PSACI_GL_BOOL = 0x8B56,
    PSACI_GL_GEOMETRY_SHADER = 0x8DD9,
    PSACI_GL_COMPILE_STATUS = 0x8B81,
    PSACI_GL_FRAGMENT_SHADER = 0x8B30,
    PSACI_GL_VERTEX_SHADER = 0x8B31,
    PSACI_GL_SAMPLER_2D = 0x8B5E,
    PSACI_GL_DEBUG_SEVERITY_NOTIFICATION = 0x826B,
    PSACI_GL_TEXTURE0 = 0x84C0,
    PSACI_GL_ARRAY_BUFFER = 0x8892,
    PSACI_GL_ELEMENT_ARRAY_BUFFER = 0x8893,
    PSACI_GL_STATIC_DRAW = 0x88E4,
    PSACI_GL_DYNAMIC_DRAW = 0x88E8,
    PSACI_GL_UNIFORM_BUFFER = 0x8A11,
    PSACI_GL_DEBUG_SEVERITY_HIGH = 0x9146,
    PSACI_GL_DEBUG_SEVERITY_MEDIUM = 0x9147,
    PSACI_GL_COLOR_BUFFER_BIT = 0x00004000,
    PSACI_GL_DEPTH_BUFFER_BIT = 0x00000100,
    PSACI_GL_DEBUG_SEVERITY_LOW = 0x9148,
    PSACI_GL_DEBUG_OUTPUT = 0x92E0,
};

SACI_API SaciU32 psaci_gl_type_to_gl(const SaciDataType data_type);

SACI_API void psaci_gl_load(void);
SACI_API void psaci_gl_enable(SaciU32);
SACI_API void psaci_gl_uniform_set_value(const SaciS32 location, SaciDataType type, const void* value);
SACI_API SaciU32 psaci_gl_create_index_buffer_dynamic(SaciU64 indice_amount, SaciU32* indices);
SACI_API SaciU32 psaci_gl_create_index_buffer_static(SaciU64 indice_amount, SaciU32* indices);
SACI_API SaciU32 psaci_gl_create_vertex_buffer_dynamic(SaciU64 size, const void* data);
SACI_API SaciU32 psaci_gl_create_vertex_buffer_static(SaciU64 size, const void* data);
SACI_API void psaci_gl_set_vertex_attrib_pointer(SaciU32 index, int size, SaciU32 type, SaciBool normalized,
                                                 SaciU64 stride, void* ptr);
SACI_API void psaci_gl_create_vertex_array(SaciU64 amount, SaciU32* arrays);
SACI_API void psaci_gl_resize_vertex_buffer(SaciU32 vao_id, SaciU32 vbo_id, SaciU64 new_size);
SACI_API void psaci_gl_bind_vertex_array(SaciU32 array);
SACI_API void psaci_gl_bind_vertex_buffer(SaciU32 vbo);
SACI_API void psaci_gl_bind_index_buffer(SaciU32 ibo);
SACI_API void psaci_gl_set_vertex_attrib_pos(SaciU32 index, SaciS32 size, SaciU32 type, SaciBool normalized, SaciU64 stride, void* ptr);
SACI_API void psaci_gl_enable_vertex_attrib_array(SaciU32 id);
SACI_API void psaci_gl_vertex_attrib_divisor(SaciU32 id, SaciU32 div);
SACI_API SaciS32 psaci_gl_uniform_location(SaciShaderId program_id, const char* const name);
SACI_API void psaci_gl_clear_color(const SaciColor color);
SACI_API void psaci_gl_clear_depth_buffer(void);
SACI_API void psaci_gl_use_program(SaciU32 program);
SACI_API void psaci_gl_set_vertex_buffer_subdata(SaciS64 offset, SaciU64 size, const void* data);
SACI_API void psaci_gl_set_index_buffer_subdata(SaciS64 offset, SaciU64 size, const void* data);
SACI_API void psaci_gl_draw_elements(SaciU32 primitives, SaciU64 count, SaciU32 type, void* data, SaciU64 instance_count);

// Textures
SACI_API void psaci_gl_generate_textures(SaciS32 count, SaciU32* tex_out);
SACI_API void psaci_gl_generate_mipmap_2d(const SaciU32 tex);
SACI_API void psaci_gl_upload_texture_2d(const SaciU32 tex, SaciS32 format, SaciS32 width, SaciS32 height, const void* data);
SACI_API void psaci_gl_get_texture_size_2d(const SaciU32 tex, SaciS32* width_out, SaciS32* height_out);
SACI_API void psaci_gl_delete_texture(SaciS32 count, SaciU32* tex_array_out);
SACI_API void psaci_gl_bind_texture_2d(enum PSaciGLConstants texture_loc, const SaciU32 tex);

// TODO bind program

SACI_API void psaci_gl_initialized_debugger(void* debug_func);

/* === Shader === */

SACI_API SaciShaderId psaci_gl_shader_compile_shader_vert(const char* source);
SACI_API SaciShaderId psaci_gl_shader_compile_shader_frag(const char* source);
SACI_API SaciShaderId psaci_gl_shader_compile_shader_geom(const char* source);
SACI_API SaciShaderId psaci_gl_shader_create_shader_program(SaciShaderId vshader, SaciShaderId fshader);
SACI_API SaciShaderId psaci_gl_shader_create_shader_program_geom(SaciShaderId vshader, SaciShaderId fshader,
                                                                 SaciShaderId gshader);
SACI_API SaciShaderId psaci_gl_shader_create_shader_program_source(const char* v, const char* f, const char* g);

#endif // SACI_BACKEND_SB_GL_H
