#ifndef SACI_BACKEND_SB_GL_H
#define SACI_BACKEND_SB_GL_H

#include "saci-utils/su-types-common.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-types-common.h"
#include "saci-utils/su-log.h"

#include "./sb-render-funcs.h"

enum sb_GLConstants {
    sb_GL_TRIANGLES = 0x0004,
    sb_GL_DEPTH_TEST = 0x0B71,
    sb_GL_TEXTURE_2D = 0x0DE1,
    sb_GL_TEXTURE_WIDTH = 0x1000,
    sb_GL_TEXTURE_HEIGHT = 0x1001,
    sb_GL_UNSIGNED_BYTE = 0x1401,
    sb_GL_UNSIGNED_INT = 0x1405,
    sb_GL_FLOAT = 0x1406,
    sb_GL_TEXTURE_BINDING_2D = 0x8069,
    sb_GL_DEBUG_OUTPUT_SYNCHRONOUS = 0x8242,
    sb_GL_DEBUG_SOURCE_API = 0x8246,
    sb_GL_DEBUG_SOURCE_WINDOW_SYSTEM = 0x8247,
    sb_GL_DEBUG_SOURCE_SHADER_COMPILER = 0x8248,
    sb_GL_DEBUG_SOURCE_THIRD_PARTY = 0x8249,
    sb_GL_DEBUG_SOURCE_APPLICATION = 0x824A,
    sb_GL_DEBUG_SOURCE_OTHER = 0x824B,
    sb_GL_DEBUG_TYPE_ERROR = 0x824C,
    sb_GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR = 0x824D,
    sb_GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR = 0x824E,
    sb_GL_DEBUG_TYPE_PORTABILITY = 0x824F,
    sb_GL_DEBUG_TYPE_PERFORMANCE = 0x8250,
    sb_GL_DEBUG_TYPE_OTHER = 0x8251,
    sb_GL_DEBUG_TYPE_MARKER = 0x8268,
    sb_GL_UNSIGNED_SHORT = 0x1403,
    sb_GL_BYTE = 0x1400,
    sb_GL_SHORT = 0x1402,
    sb_GL_INT = 0x1404,
    sb_GL_BOOL = 0x8B56,
    sb_GL_GEOMETRY_SHADER = 0x8DD9,
    sb_GL_COMPILE_STATUS = 0x8B81,
    sb_GL_FRAGMENT_SHADER = 0x8B30,
    sb_GL_VERTEX_SHADER = 0x8B31,
    sb_GL_SAMPLER_2D = 0x8B5E,
    sb_GL_DEBUG_SEVERITY_NOTIFICATION = 0x826B,
    sb_GL_TEXTURE0 = 0x84C0,
    sb_GL_ARRAY_BUFFER = 0x8892,
    sb_GL_ELEMENT_ARRAY_BUFFER = 0x8893,
    sb_GL_STATIC_DRAW = 0x88E4,
    sb_GL_DYNAMIC_DRAW = 0x88E8,
    sb_GL_UNIFORM_BUFFER = 0x8A11,
    sb_GL_DEBUG_SEVERITY_HIGH = 0x9146,
    sb_GL_DEBUG_SEVERITY_MEDIUM = 0x9147,
    sb_GL_COLOR_BUFFER_BIT = 0x00004000,
    sb_GL_DEPTH_BUFFER_BIT = 0x00000100,
    sb_GL_DEBUG_SEVERITY_LOW = 0x9148,
    sb_GL_DEBUG_OUTPUT = 0x92E0,
};

SA_API su_U32 sb_gl_type_to_gl(const su_DataType data_type);

SA_API void sb_gl_load(void);
SA_API void sb_gl_uniform_set_value(const su_S32 location, su_DataType type, const void* value);
SA_API su_U32 sb_gl_create_index_buffer_dynamic(su_U64 indice_amount, su_U32* indices);
SA_API su_U32 sb_gl_create_index_buffer_static(su_U64 indice_amount, su_U32* indices);
SA_API su_U32 sb_gl_create_vertex_buffer_dynamic(su_U64 size, const void* data);
SA_API su_U32 sb_gl_create_vertex_buffer_static(su_U64 size, const void* data);
SA_API void sb_gl_create_vertex_array(su_U64 amount, su_U32* arrays);
SA_API void sb_gl_resize_vertex_buffer(su_U32 vao_id, su_U32 vbo_id, su_U64 new_size);
SA_API void sb_gl_bind_vertex_array(su_U32 array);
SA_API void sb_gl_bind_vertex_buffer(su_U32 vbo);
SA_API void sb_gl_set_vertex_attrib_pointer(su_U32 index, int size, su_U32 type, su_Bool normalized, su_U64 stride, void* ptr);
SA_API void sb_gl_enable_vertex_attrib_array(su_U32 id);
SA_API void sb_gl_vertex_attrib_divisor(su_U32 id, su_U32 div);
SA_API su_S32 sb_gl_uniform_location(su_ShaderId program_id, const char* const name);
// TODO bind program

/* === Shader === */

SA_API su_ShaderId sb_gl_shader_compile_shader_vert(const char* source);
SA_API su_ShaderId sb_gl_shader_compile_shader_frag(const char* source);
SA_API su_ShaderId sb_gl_shader_compile_shader_geom(const char* source);
SA_API su_ShaderId sb_gl_shader_create_shader_program(su_ShaderId vshader, su_ShaderId fshader);
SA_API su_ShaderId sb_gl_shader_create_shader_program_geom(su_ShaderId vshader, su_ShaderId fshader,
                                                           su_ShaderId gshader);

#endif // SACI_BACKEND_SB_GL_H
