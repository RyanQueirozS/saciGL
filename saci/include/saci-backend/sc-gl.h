#ifndef SACI_CORE_SC_GL_H
#define SACI_CORE_SC_GL_H

#include <GLFW/glfw3.h>

#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"

/* === OpenGL === */

SA_API void sc_GL_Uniform_Set_Value(const su_s32 location, su_dataType type, const void* value);
SA_API su_u32 sc_GL_Create_Index_Buffer_Dynamic(su_u32* indices, su_u64 indice_amount);
SA_API su_u32 sc_GL_Create_Index_Buffer_Static(su_u32* indices, su_u64 indice_amount);
SA_API su_u32 sc_GL_Create_Vertex_Buffer(su_u64 size, const void* data, su_u32 usage);
SA_API void sc_GL_Create_Vertex_Array(su_u64 amount, su_u32* arrays);
SA_API void sc_GL_Resize_Vertex_Buffer(su_u32 vao_id, su_u32 vbo_id, su_u64 new_size);
SA_API void sc_GL_Bind_Vertex_Array(su_u32 array);
SA_API void sc_GL_Bind_Vertex_Buffer(su_u32 vbo);
SA_API void sc_GL_Set_Vertex_Attrib_Pointer(su_u32 index, int size, su_u32 type, su_bool normalized, su_u64 stride, void* ptr);
SA_API void sc_GL_Enable_Vertex_Attrib_Array(su_u32 id);
SA_API su_s32 sc_GL_Uniform_Location(su_shaderId program_id, const char* const name);
// TODO bind program

/* === Shader === */

SA_API su_u32 sc_Shader_Compile_Shader_Vert(const char* source);
SA_API su_u32 sc_Shader_Compile_Shader_Frag(const char* source);
SA_API su_u32 sc_Shader_Compile_Shader_Geom(const char* source);
SA_API su_u32 sc_Shader_Create_Shader_Program(su_shaderId vshader, su_shaderId fshader);
SA_API su_u32 sc_Shader_Create_Shader_Program_Geom(su_shaderId vshader, su_shaderId fshader,
                                                   su_shaderId gshader);

#endif // SACI_CORE_SC_GL_H_
