#ifndef SACI_BACKEND_RENDERER_SB_RENDERER_H
#define SACI_BACKEND_RENDERER_SB_RENDERER_H

#include "../sb-backend-common.h"
#include "saci-utils/su-types-common.h"
#include "saci-utils/su-general.h"
#include "saci-utils/memory/su-darray.h"

typedef struct sb_Renderer sb_Renderer;

#ifdef SB_RENDERER_STRUCT_EXPOSE
#  ifndef SB_RENDERER_STRUCT
#    define SB_RENDERER_STRUCT

struct sb_vertex {
    sa_vec3 pos;
    sa_color color;
    sa_uv uv;
};

// The fields are structured in a way that enforces minimum memory change over time
struct sb_renderer {
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

    struct sb_renderBatch {
        sa_u64 uniform_struct_block_size;
        sa_textureId texture;
        sa_u32 index_array_length;
        sa_u32 vertex_array_length;
        sa_u32* index_array;
        struct sb_vertex* vertex_array;
        sa_u8* uniform_struct_block;
    }* batch_array;

    sa_u8* uniform_struct_block;

    sa_u32* bound_index_array_buffer;
};

#  endif // SB_RENDERER_STRUCT
#endif   // SB_RENDERER_STRUCT_EXPOSE

#define sb_RENDERER_FREE_OPT_MEMORY 0b01
#define sb_RENDERER_FREE_OPT_OPENGL 0b10

// TODO
#if 0
#  define sb_RENDERER_UNIFORM_FLAG_
#  define sb_RENDERER_UNIFORM_FLAG_
#endif

#if 0
#  define sb_RENDERER_BATCH_OVERFLOW_ACTION_FLUSH 01
#  define sb_RENDERER_BATCH_OVERFLOW_ACTION_CRASH 02
#  define sb_RENDERER_BATCH_OVERFLOW_ACTION_SKIP 03
#  define sb_RENDERER_BATCH_OVERFLOW_ACTION_RESIZE 04

#  define sb_RENDERER_CALL_OVERFLOW_ACTION_FLUSH 01
#  define sb_RENDERER_CALL_OVERFLOW_ACTION_CRASH 02
#  define sb_RENDERER_CALL_OVERFLOW_ACTION_SKIP 03
#  define sb_RENDERER_CALL_OVERFLOW_ACTION_RESIZE 04
#endif // TODO to be implemented

enum sb_RendererType {
    sb_RENDERER_STATIC,
    sb_RENDERER_DYNAMIC,
    sb_RENDERER_INSTANCE,
};

SA_API sb_Renderer* sb_renderer_new(const enum sb_RendererType type);

SA_API void sb_renderer_begin(struct sb_Renderer* rendr);

SA_API void sb_renderer_bind_texture(struct sb_Renderer* rendr,
                                     const su_TextureId tex_id);

SA_API su_S32 sb_renderer_get_uniform_id(struct sb_Renderer* rendr,
                                         const char* const uniform_name);

SA_API void sb_renderer_set_uniform(struct sb_Renderer* rendr,
                                    const su_S32 uniform_id,
                                    const void* const value,
                                    const su_DataType type);

SA_API void sb_renderer_bind_index_buffer(struct sb_Renderer* rendr,
                                          const su_DArray* new_indices);

SA_API void sb_renderer_push_mesh(struct sb_Renderer* rendr,
                                  const su_DArray* pos_array,
                                  const su_DArray* uv_array,
                                  const su_DArray* color_array);

SA_API void sb_renderer_draw(const struct sb_Renderer* rendr);

SA_API void sb_renderer_free(struct sb_Renderer* rendr);

SA_API void sb_renderer_free_opts(struct sb_Renderer* rendr, int free_opts);

/* --- Renderer specific --- */

// This needs to be used with a instanced renderer, else it should crash
SA_API void sb_renderer_push_instance_data(struct sb_Renderer* rendr, const struct sb_DrawInstanceData* instance_data);

SA_API void sb_renderer_set_instance_data_array(struct sb_Renderer* rendr, const su_DArray* instance_data_array);

#endif // SACI_BACKEND_RENDERER_SB_RENDERER_H
