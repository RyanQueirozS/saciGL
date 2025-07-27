#ifndef SACI_BACKEND_SB_GL_H
#define SACI_BACKEND_SB_GL_H

#include <GLFW/glfw3.h>

#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"

/* === OpenGL === */

SA_API void sb_gl_uniform_set_value(const su_s32 location, su_dataType type, const void* value);
SA_API su_u32 sb_gl_create_index_buffer_dynamic(su_u32* indices, su_u64 indice_amount);
SA_API su_u32 sb_gl_create_index_buffer_static(su_u32* indices, su_u64 indice_amount);
SA_API su_u32 sb_gl_create_vertex_buffer(su_u64 size, const void* data, su_u32 usage);
SA_API void sb_gl_create_vertex_array(su_u64 amount, su_u32* arrays);
SA_API void sb_gl_resize_vertex_buffer(su_u32 vao_id, su_u32 vbo_id, su_u64 new_size);
SA_API void sb_gl_bind_vertex_array(su_u32 array);
SA_API void sb_gl_bind_vertex_buffer(su_u32 vbo);
SA_API void sb_gl_set_vertex_attrib_pointer(su_u32 index, int size, su_u32 type, su_bool normalized, su_u64 stride, void* ptr);
SA_API void sb_gl_enable_vertex_attrib_array(su_u32 id);
SA_API su_s32 sb_gl_uniform_location(su_shaderId program_id, const char* const name);
// TODO bind program

/* === Shader === */

SA_API su_u32 sb_shader_compile_shader_vert(const char* source);
SA_API su_u32 sb_shader_compile_shader_frag(const char* source);
SA_API su_u32 sb_shader_compile_shader_geom(const char* source);
SA_API su_u32 sb_shader_create_shader_program(su_shaderId vshader, su_shaderId fshader);
SA_API su_u32 sb_shader_create_shader_program_geom(su_shaderId vshader, su_shaderId fshader,
                                                   su_shaderId gshader);

#endif // SACI_BACKEND_SB_GL_H_
