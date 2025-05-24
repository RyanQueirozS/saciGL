#ifndef SACI_CORE_SC_RENDERING_H
#define SACI_CORE_SC_RENDERING_H

#include <GLFW/glfw3.h>

#include "saci-core/sc-model.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-darray-internal.h"
#include "saci-utils/su-darray.h"
#include "saci-utils/su-general.h"

/* === Renderer === */

typedef struct sc_renderer sc_renderer;

#ifdef SC_RENDERER_STRUCT_EXPOSE
#  ifndef SC_RENDERER_STRUCT
#    define SC_RENDERER_STRUCT

struct sc_vertex {
    sa_vec3 pos;
    sa_color color;
    sa_uv uv;
};

// The fields are structured in a way that enforces minimum memory change over time
struct sc_renderer {
    sa_textureId bound_texture_id;
    sa_u32 bound_index_array_length;
    sa_u32 bound_index_array_capacity;
    sa_u64 bound_uniform_struct_size;

    sa_shaderId shader_program;
    sa_bufferId ibo, ubo, vbo, vao;

    sa_u32 batch_index_capacity;
    sa_u32 batch_vertex_capacity;
    sa_u8 batch_array_capacity;
    sa_u8 batch_in_use;

    struct sc_renderBatch {
        sa_u64 uniform_struct_block_size;
        sa_textureId texture;
        sa_u32 index_array_length;
        sa_u32 vertex_array_length;
        sa_u32* index_array;
        struct sc_vertex* vertex_array;
        sa_u8* uniform_struct_block;
    }* batch_array;

    sa_u8* uniform_struct_block;

    sa_u32* bound_index_array_buffer;
};

#  endif // SC_RENDERER_STRUCT
#endif   // SC_RENDERER_STRUCT_EXPOSE

#define sc_RENDERER_FREE_OPT_MEMORY 0b01
#define sc_RENDERER_FREE_OPT_OPENGL 0b10

#define sc_RENDERER_UNIFORM_FLAG_IS_2D 0b0
#define sc_RENDERER_UNIFORM_FLAG_IS_3D 0b1

#if 0
#  define sc_RENDERER_BATCH_OVERFLOW_ACTION_FLUSH 01
#  define sc_RENDERER_BATCH_OVERFLOW_ACTION_CRASH 02
#  define sc_RENDERER_BATCH_OVERFLOW_ACTION_SKIP 03
#  define sc_RENDERER_BATCH_OVERFLOW_ACTION_RESIZE 04

#  define sc_RENDERER_CALL_OVERFLOW_ACTION_FLUSH 01
#  define sc_RENDERER_CALL_OVERFLOW_ACTION_CRASH 02
#  define sc_RENDERER_CALL_OVERFLOW_ACTION_SKIP 03
#  define sc_RENDERER_CALL_OVERFLOW_ACTION_RESIZE 04
#endif // TODO to be implemented

SA_API sc_renderer* sc_Renderer_New_Default(void);

SA_API sc_renderer* sc_Renderer_New_Default_Ctx(void* mem_ctx,
                                                sa_u64 batch_index_capacity,
                                                sa_u64 batch_vertex_capacity,
                                                sa_u64 bound_capacity);

SA_API sc_renderer* sc_Renderer_New_From_Config(const char* file_path);

SA_API void sc_Renderer_Begin(struct sc_renderer* rendr);

SA_API void sc_Renderer_Bind_Texture(struct sc_renderer* rendr,
                                     const sa_textureId tex_id);

SA_API void sc_Renderer_Set_Uniform(struct sc_renderer* rendr,
                                    const sa_u32 uniform_id,
                                    const void* const value,
                                    const sa_dataType type);

SA_API void sc_Renderer_Bind_Index_Buffer(struct sc_renderer* rendr,
                                          const sa_u32Array* new_indices);

SA_API void sc_Renderer_Push_Mesh_Dynamic(struct sc_renderer* rendr,
                                          const sa_vec3Array* position_array,
                                          const sa_uvArray* uv_array,
                                          const sa_colorArray* color_array);

SA_API void sc_Renderer_Push_Mesh_Instanced(struct sc_renderer* rendr,
                                            const sa_vec3* pos_array,
                                            const sa_uv* uv_array,
                                            const sa_color* color_array,
                                            const sa_mat4* instance_transform,
                                            const sa_u32 vertex_count,
                                            const sa_u32 instance_count);

SA_API void sc_Renderer_Push_Model_Mesh(struct sc_renderer* rendr,
                                        const sc_modelMesh* model_mesh);

SA_API void sc_Renderer_End(struct sc_renderer* rendr);

SA_API void sc_Renderer_Free(struct sc_renderer* rendr);

SA_API sa_u32 sc_Renderer_Get_Uniform_Id(struct sc_renderer* rendr,
                                         const char* const uniform_name);

SA_API void sc_Renderer_Free_Opts(struct sc_renderer* rendr, int free_opts);

/* === OpenGL === */

SA_API void sc_GL_Uniform_Set_Value(const sa_u32 location, sa_dataType type, const void* value);
SA_API sa_u32 sc_GL_Create_Index_Buffer_Dynamic(sa_u32* indices, sa_u64 indice_amount);
SA_API sa_u32 sc_GL_Create_Index_Buffer_Static(sa_u32* indices, sa_u64 indice_amount);
SA_API sa_u32 sc_GL_Create_Vertex_Buffer(sa_u64 size, const void* data, sa_u32 usage);
SA_API void sc_GL_Create_Vertex_Array(sa_u64 size, sa_u32* arrays);
SA_API void sc_GL_Resize_Vertex_Buffer(sa_u32 vao_id, sa_u32 vbo_id, sa_u64 new_size);
SA_API void sc_GL_Bind_Vertex_Array(sa_u32 array);
SA_API void sc_GL_Bind_Vertex_Buffer(sa_u32 vbo);
SA_API void sc_GL_Set_Vertex_Attrib_Pointer(sa_u32 index, int size, sa_u32 type, sa_bool normalized, sa_u64 stride, void* ptr);
SA_API void sc_GL_Enable_Vertex_Attrib_Array(sa_u32 id);
// TODO bind program

/* === Shader === */

SA_API sa_u32 sc_Shader_Compile_Shader_Vert(const char* source);
SA_API sa_u32 sc_Shader_Compile_Shader_Frag(const char* source);
SA_API sa_u32 sc_Shader_Compile_Shader_Geom(const char* source);
SA_API sa_u32 sc_Shader_Create_Shader_Program(sa_shaderId vshader, sa_shaderId fshader);
SA_API sa_u32 sc_Shader_Create_Shader_Program_Geom(sa_shaderId vshader, sa_shaderId fshader,
                                                   sa_shaderId gshader);

#endif // SACI_CORE_SC_RENDERING_H_
