#ifndef SACI_CORE_SC_GL_H
#define SACI_CORE_SC_GL_H

#include <GLFW/glfw3.h>

#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"

/* === OpenGL === */

SA_API void sc_GL_Uniform_Set_Value(const sa_s32 location, sa_dataType type, const void* value);
SA_API sa_u32 sc_GL_Create_Index_Buffer_Dynamic(sa_u32* indices, sa_u64 indice_amount);
SA_API sa_u32 sc_GL_Create_Index_Buffer_Static(sa_u32* indices, sa_u64 indice_amount);
SA_API sa_u32 sc_GL_Create_Vertex_Buffer(sa_u64 size, const void* data, sa_u32 usage);
SA_API void sc_GL_Create_Vertex_Array(sa_u64 size, sa_u32* arrays);
SA_API void sc_GL_Resize_Vertex_Buffer(sa_u32 vao_id, sa_u32 vbo_id, sa_u64 new_size);
SA_API void sc_GL_Bind_Vertex_Array(sa_u32 array);
SA_API void sc_GL_Bind_Vertex_Buffer(sa_u32 vbo);
SA_API void sc_GL_Set_Vertex_Attrib_Pointer(sa_u32 index, int size, sa_u32 type, sa_bool normalized, sa_u64 stride, void* ptr);
SA_API void sc_GL_Enable_Vertex_Attrib_Array(sa_u32 id);
SA_API sa_s32 sc_GL_Uniform_Location(sa_shaderId program_id, const char* const name);
// TODO bind program

/* === Shader === */

SA_API sa_u32 sc_Shader_Compile_Shader_Vert(const char* source);
SA_API sa_u32 sc_Shader_Compile_Shader_Frag(const char* source);
SA_API sa_u32 sc_Shader_Compile_Shader_Geom(const char* source);
SA_API sa_u32 sc_Shader_Create_Shader_Program(sa_shaderId vshader, sa_shaderId fshader);
SA_API sa_u32 sc_Shader_Create_Shader_Program_Geom(sa_shaderId vshader, sa_shaderId fshader,
                                                   sa_shaderId gshader);

#endif // SACI_CORE_SC_GL_H_
