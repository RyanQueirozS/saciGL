#ifndef SACI_CORE_SC_RENDERER_H
#define SACI_CORE_SC_RENDERER_H

#include "saci-backend/sc-model.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"

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

// TODO
#if 0
#  define sc_RENDERER_UNIFORM_FLAG_
#  define sc_RENDERER_UNIFORM_FLAG_
#endif

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

enum sc_rendererType {
    sc_RENDERER_STATIC,
    sc_RENDERER_DYNAMIC,
    sc_RENDERER_INSTANCE,
};

SA_API sc_renderer* sc_Renderer_New(const enum sc_rendererType type);

SA_API void sc_Renderer_Begin(struct sc_renderer* rendr);

SA_API void sc_Renderer_Bind_Texture(struct sc_renderer* rendr,
                                     const sa_textureId tex_id);

SA_API sa_s32 sc_Renderer_Get_Uniform_Id(struct sc_renderer* rendr,
                                         const char* const uniform_name);

SA_API void sc_Renderer_Set_Uniform(struct sc_renderer* rendr,
                                    const sa_s32 uniform_id,
                                    const void* const value,
                                    const sa_dataType type);

SA_API void sc_Renderer_Bind_Index_Buffer(struct sc_renderer* rendr,
                                          const su_dArray* new_indices);

SA_API void sc_Renderer_Push_Mesh(struct sc_renderer* rendr,
                                  const su_dArray* pos_array,
                                  const su_dArray* uv_array,
                                  const su_dArray* color_array);

SA_API void sc_Renderer_Draw(const struct sc_renderer* rendr);

SA_API void sc_Renderer_Free(struct sc_renderer* rendr);

SA_API void sc_Renderer_Free_Opts(struct sc_renderer* rendr, int free_opts);

/* --- Renderer specific --- */

// This needs to be used with a instanced renderer, else it should crash
SA_API void sc_Renderer_Set_Instance_Transforms(struct sc_renderer* rendr, su_dArray* transform_array);

#endif // SACI_CORE_SC_RENDERER_H
