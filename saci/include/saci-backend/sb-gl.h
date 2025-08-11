#ifndef SACI_BACKEND_SB_GL_H
#define SACI_BACKEND_SB_GL_H

#include <GLFW/glfw3.h>

#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"

/* === OpenGL === */

SA_API void sb_gl_load(void);
SA_API void sb_gl_uniform_set_value(const su_S32 location, su_DataType type, const void* value);
SA_API su_U32 sb_gl_create_index_buffer_dynamic(su_U32* indices, su_U64 indice_amount);
SA_API su_U32 sb_gl_create_index_buffer_static(su_U32* indices, su_U64 indice_amount);
SA_API su_U32 sb_gl_create_vertex_buffer(su_U64 size, const void* data, su_U32 usage);
SA_API void sb_gl_create_vertex_array(su_U64 amount, su_U32* arrays);
SA_API void sb_gl_resize_vertex_buffer(su_U32 vao_id, su_U32 vbo_id, su_U64 new_size);
SA_API void sb_gl_bind_vertex_array(su_U32 array);
SA_API void sb_gl_bind_vertex_buffer(su_U32 vbo);
SA_API void sb_gl_set_vertex_attrib_pointer(su_U32 index, int size, su_U32 type, su_Bool normalized, su_U64 stride, void* ptr);
SA_API void sb_gl_enable_vertex_attrib_array(su_U32 id);
SA_API su_S32 sb_gl_uniform_location(su_ShaderId program_id, const char* const name);
// TODO bind program

/* === Shader === */

SA_API su_U32 sb_shader_compile_shader_vert(const char* source);
SA_API su_U32 sb_shader_compile_shader_frag(const char* source);
SA_API su_U32 sb_shader_compile_shader_geom(const char* source);
SA_API su_U32 sb_shader_create_shader_program(su_ShaderId vshader, su_ShaderId fshader);
SA_API su_U32 sb_shader_create_shader_program_geom(su_ShaderId vshader, su_ShaderId fshader,
                                                   su_ShaderId gshader);

#endif // SACI_BACKEND_SB_GL_H
