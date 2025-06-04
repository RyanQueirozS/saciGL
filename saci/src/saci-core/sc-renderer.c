#include <glad/glad.h>

#include "saci-core/sc-renderer.h"

#include "saci-core/sc-gl.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-types.h"

#include "saci-core/sc-texture.h"
#include "saci-core/sc-config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_ASSERT(x) sa_Log_Assert_Message_m(x, "Error in arena function")
#define ARENA_FREE(x) sa_Free_m(x)
#define ARENA_MALLOC(x) sa_Malloc_m(x)
#define ARENA_ALLOCATOR_IMPL
#define ARENA_DEBUG
#include "arena/arena.h"

/* === Renderer === */

// TODO to be used
#ifndef SC_TEXTURE_ARRAY_SIZE
#  ifdef _WIN32
// Windows
#    define SC_TEXTURE_ARRAY_SIZE 16
#  elif defined(__APPLE__) && defined(__MACH__)
// macOS
#    define SC_TEXTURE_ARRAY_SIZE 32
#  elif defined(__linux__)
// Linux
#    define SC_TEXTURE_ARRAY_SIZE 16
#  elif defined(__ANDROID__)
// Android
#    define SC_TEXTURE_ARRAY_SIZE 8
#  else
// Default case
#    define SC_TEXTURE_ARRAY_SIZE 8
#  endif // _WIN32
#endif   // SC_TEXTURE_ARRAY_SIZE

#ifndef SC_RENDERER_UNIFORMS
#  define SC_RENDERER_UNIFORMS

// TODO test these
// These might change and might not be used later...
#  define SC_U_MODEL_MATRIX_LOC 0
#  define SC_U_VIEW_MATRIX_LOC 1
#  define SC_U_PROJECTION_MATRIX_LOC 2
#  define SC_U_FLAGS 3
#  define SC_U_LIGHTING 4
#  define SC_U_TEXTURE_LOC 5
#  define SC_U_USE_TEXTURE_LOC 6

#endif // SC_RENDERER_UNIFORMS

#ifndef SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY
#  define SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY (16000)
#endif // SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY

#ifndef SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY
#  define SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY sa_Scast_To_m(sa_u64)(SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY * 6 / 4)
#endif // SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY

#ifndef SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY
#  define SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY
#endif // SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY

#ifndef SC_RENDERER_DEFAULT_BATCH_CAPACITY
#  define SC_RENDERER_DEFAULT_BATCH_CAPACITY (10)
#endif // SC_RENDERER_DEFAULT_BATCH_CAPACITY

#ifndef SC_RENDERER_DEFAULT_UBO_SIZE
#  define SC_RENDERER_DEFAULT_UBO_SIZE (212)
#endif // SC_RENDERER_DEFAULT_UBO_SIZE

#ifndef SC_RENDERER_DEFAULT_UBO_BINDING_POINT
#  define SC_RENDERER_DEFAULT_UBO_BINDING_POINT 0
#endif // SC_RENDERER_DEFAULT_UBO_BINDING_POINT

#ifndef SC_RENDERER_DEFAULT_RENDERER_CONFIG_PATH
#  define SC_RENDERER_DEFAULT_RENDERER_CONFIG_PATH "renderer-config.lua"
#endif // SC_RENDERER_DEFAULT_RENDERER_CONFIG_PATH

#ifndef SC_RENDERER_MAX_DYNAMIC_VERT_PER_PUSH
#  define SC_RENDERER_MAX_DYNAMIC_VERT_PER_PUSH (300)
#endif // SC_RENDERER_MAX_DYNAMIC_VERT_PER_PUSH

SA_INTERNAL const char* const sc_VERT_SHADER =
    // TODO Evaluate if there should be multiple shaders, one for 2d, 3d
    // instancing, 3d dynamic mesh etc. Saci-Core needs to be
    // performatic but also "just work", so the user shouldn't need to
    // change it unless they want that '1%' boost
    "#version 330 core\n"

    "layout (location = 0) in vec3 a_pos;\n"
    "layout (location = 1) in vec4 a_color;\n"
    "layout (location = 2) in vec2 a_texcoord;\n"
    "layout (location = 3) in mat4 a_model_matrix;\n"

    "uniform mat4 u_model_matrix;\n"
    "uniform mat4 u_view_matrix;\n"
    "uniform mat4 u_projection_matrix;\n"
    "uniform int u_flags;\n"
    "uniform vec4 u_lighting;\n"

    "out vec4 v_color;\n"
    "out vec2 v_texcoord;\n"

    "void main()\n"
    "{\n"
    "   vec4 world_position = u_model_matrix * a_model_matrix * vec4(a_pos, 1.0);\n"
    "   if ((u_flags & 0x1) == 1){\n"
    "       gl_Position = u_projection_matrix * u_view_matrix * world_position;\n"
    "   } else {\n"
    "       gl_Position = world_position;\n"
    "   }\n"
    "   v_color = a_color;\n"
    "   v_texcoord = a_texcoord;\n"
    "}\n\0";

SA_INTERNAL const char* const sc_FRAG_SHADER =
    "#version 330 core\n"

    "in vec4 v_color;\n"
    "in vec2 v_texcoord;\n"

    "uniform sampler2D u_texture;\n"
    "uniform bool u_use_texture;\n"

    "out vec4 frag_color;\n"

    "void main()\n"
    "{\n"
    "   if (u_use_texture) {\n"
    "       vec4 texcolor = texture(u_texture, v_texcoord);\n"
    "       frag_color = texcolor * v_color;\n"
    "   } else {\n"
    "       frag_color = v_color;\n"
    "   }\n"
    "}\n\0";

#ifndef SC_RENDERER_STRUCT
#  define SC_RENDERER_STRUCT

struct sc_vertex {
    sa_vec3 pos;
    sa_color color;
    sa_uv uv;
};

union sc_uniformValue {
    // Scalar types
    sa_u8 u8;
    sa_u16 u16;
    sa_u32 u32;
    sa_u64 u64;
    sa_s8 s8;
    sa_s16 s16;
    sa_s32 s32;
    sa_s64 s64;
    sa_bool boolean;

    sa_uv uv;
    sa_vec2 vec2;
    sa_vec3 vec3;
    sa_vec4 vec4;

    // Color
    sa_color color;

    // TODO NEED TO BE ADDED
    sa_mat4 mat4;

    // float mat2[2][2];
    // float mat3[3][3];
    // float mat2x3[2][3];
    // float mat2x4[2][4];
    // float mat3x2[3][2];
    // float mat3x4[3][4];
    // float mat4x2[4][2];
    // float mat4x3[4][3];
};

struct sc_rendererUniformData {
    sa_dataType type;
    sa_s32 location;
    union sc_uniformValue value;
};

struct sc_rendererBoundInfo {
    sa_u64 index_array_max_length;
    sa_u64 uniform_array_max_length;
};

struct sc_instanceBatch {
    sa_textureId texture;
    sa_dArray* model_matrix_array;
    sa_dArray* index_array;
    sa_dArray* vertex_array;
    sa_dArray* uniform_data;
};

struct sc_staticBatch {
    sa_textureId texture;
    sa_dArray* index_array;
    sa_dArray* vertex_array;
    sa_dArray* uniform_data;
};

struct sc_instanceBatchInfo {
    sa_u64 index_array_max;
    sa_u64 vertex_array_max;
    sa_u64 transform_array_max;
    sa_u64 batch_struct_allocation_size;
    sa_u8 batch_length_max;
    sa_u8 in_use;
};

struct sc_staticBatchInfo {
    sa_u64 vertex_array_max;
    sa_u64 batch_struct_allocation_size;
    sa_u64 index_array_max;
    sa_u8 batch_length_max;
    sa_u8 in_use;
};

struct sc_rendererConfig {
    struct sc_instanceBatchInfo instance_batch_info;
    struct sc_staticBatchInfo static_batch_info;
    struct sc_rendererBoundInfo bound_info;
    char* v_shader_code;
    char* f_shader_code;
    char* g_shader_code;
};

// The fields are structured in a way that enforces minimum memory change over time
struct sc_renderer {
    sa_shaderId shader_program;
    sa_bufferId ibo, vbo, vao;
    sa_bufferId instance_vbo; // used for instancing

    struct sc_rendererBoundInfo bound_info;

    struct sc_staticBatchInfo static_batch_info;

    struct sc_instanceBatchInfo instance_batch_info;

    struct sc_rendererBound {
        sa_textureId texture;
        sa_dArray* index_array;
        sa_dArray* uniform_data_array;
    } bound;

    Arena instance_batch_arena;
    struct sc_instanceBatch** instance_batch_address_array; // Stores ptrs to where each batch is allocated in the arena, where the idx:N is batch:N

    Arena static_batch_arena;
    struct sc_staticBatch** static_batch_address_array;
};

#endif // SC_RENDERER_STRUCT

/* --- Renderer Helper --- */

// Frees opengl related data in sc_renderer
SA_INTERNAL_INLINE void s_Renderer_Free_Opengl(struct sc_renderer* rendr);

// Frees batch related data in sc_renderer
SA_INTERNAL void s_Renderer_Free_Batch_Array(struct sc_renderer* rendr);

// Frees memory related data in sc_renderer (batch and call included)
SA_INTERNAL void s_Renderer_Free_Memory(struct sc_renderer* rendr);

// Resets bound information
SA_INTERNAL void s_Renderer_Reset_Bound(struct sc_renderer* rendr);

// Resets batch information
SA_INTERNAL void s_Renderer_Reset_Batch(struct sc_renderer* rendr);

// Validates before pushing to call buffer
SA_INTERNAL sa_bool s_Renderer_Validate_Before_Push(const struct sc_renderer* rendr,
                                                    const sa_dArray* pos_array,
                                                    const sa_dArray* uv_array,
                                                    const sa_dArray* color_array);

SA_INTERNAL void s_Renderer_Initialize_Config(struct sc_rendererConfig* rendr_cfg, const char* cfg_path);

// Initializes with default opengl data
SA_INTERNAL void s_Renderer_Init_GL(struct sc_renderer* rendr, const struct sc_rendererConfig rendr_cfg);

// Initializes with default batch data
SA_INTERNAL void s_Renderer_Init_Batch(struct sc_renderer* rendr, const struct sc_rendererConfig rendr_cfg);

SA_INTERNAL void s_Renderer_Init_Dynamic_Batch(struct sc_renderer* rendr);

SA_INTERNAL void s_Renderer_Init_Instance_Batch(struct sc_renderer* rendr);

// Appends an index buffer (dest)
SA_INTERNAL sa_u32 s_Renderer_Index_Buffer_Append_Minus_Offset(sa_u32* dest_out,
                                                               sa_u32* src,
                                                               sa_u32 dest_indices_pushed,
                                                               sa_u32 src_indices_pushed,
                                                               sa_u32 dest_capacity,
                                                               sa_u32 src_count,
                                                               sa_u32 offset);

SA_INTERNAL sa_u32 s_Renderer_Vertex_Array_Append(sa_dArray* dest_out,
                                                  const sa_dArray* src_pos,
                                                  const sa_dArray* src_uv,
                                                  const sa_dArray* src_color,
                                                  sa_u32 src_count);

SA_INTERNAL sa_bool s_Renderer_Batch_Can_Push(const sc_renderer* rendr, const struct sc_staticBatch* batch);

SA_INTERNAL union sc_uniformValue s_Renderer_Uniform_Value_From_Type(sa_dataType type, const void* value);

SA_INTERNAL void s_Renderer_Set_Uniform_From_Uniform_Data(const struct sc_rendererUniformData uniform_data);

SA_INTERNAL void s_Renderer_Draw_Dynamic_Batch(const sc_renderer* rendr);

SA_INTERNAL void s_Renderer_Draw_Instance_Batch(const sc_renderer* rendr);

/* --- Renderer Header Impl --- */

SA_API sc_renderer* sc_Renderer_New_Default(void) {
    // TODO this should return one with the default values on the macros.
    return sc_Renderer_New_From_Config(SC_RENDERER_DEFAULT_RENDERER_CONFIG_PATH);
}

// TODO
#if 0
SA_API struct sc_renderer* sc_Renderer_New_Default_Ctx(void* mem_ctx, sa_u64 batch_index_capacity, sa_u64 batch_vertex_capacity, sa_u64 bound_capacity) {
    if (!mem_ctx) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                             "Invalid memory context in sc_Renderer_New_Default_Ctx");
        return NULL; // Should crash, but if LOG_ERROR_PRINT_m is rewriten, returns NULL
    }

    // TODO
    struct sc_renderer* rendr = sa_Malloc_m(sizeof(struct sc_renderer));
    assert(rendr);
    sc_Renderer_Init(rendr);

    return rendr;
}
#endif

SA_API sc_renderer* sc_Renderer_New_From_Config(const char* file_path) {
    // TODO if this is passed a NULL path, it should use the default renderer config path
    struct sc_renderer* rendr = sa_Calloc_m(1, sizeof(struct sc_renderer));
    sa_Log_Assert_Message_m(rendr, "Renderer could not be created");
    struct sc_rendererConfig rendr_cfg = {0};
    s_Renderer_Initialize_Config(&rendr_cfg, file_path);
    s_Renderer_Init_GL(rendr, rendr_cfg);
    s_Renderer_Init_Batch(rendr, rendr_cfg);

    { // Bound info
        rendr->bound_info.index_array_max_length = SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;
        rendr->bound.index_array = sa_DArray_Create(rendr->bound_info.index_array_max_length,
                                                    sizeof(sa_u32),
                                                    sa_TRUE);
    }

    return rendr;
}

SA_API void sc_Renderer_Begin(struct sc_renderer* rendr) {
    s_Renderer_Reset_Batch(rendr);
    s_Renderer_Reset_Bound(rendr);
}

SA_API void sc_Renderer_Bind_Texture(struct sc_renderer* rendr, sa_textureId tex_id) {
    rendr->bound.texture = tex_id;
}

SA_API void sc_Renderer_Set_Uniform(struct sc_renderer* rendr,
                                    const sa_s32 uniform_id,
                                    const void* const value,
                                    const sa_dataType type) {
    if (uniform_id < 0 || uniform_id >= SA_TYPE_MAX ||
        uniform_id == SA_TYPE_BUFFERID || uniform_id == SA_TYPE_SHADERID ||
        uniform_id == SA_TYPE_TEXTUREID) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_RENDERER,
                             "Trying to bind uniform with invalid ID");
        return;
    }
    if (sa_Scast_To_m(int)(type) == 0 || sa_Scast_To_m(int)(type) > 26) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_RENDERER,
                             "Trying to bind uniform with invalid type");
        return;
    }
    struct sc_rendererUniformData new_uniform = {0};
    new_uniform.type = type;
    new_uniform.value = s_Renderer_Uniform_Value_From_Type(type, value);
    new_uniform.location = uniform_id;

    struct sc_rendererUniformData* uniform_data = sa_Malloc_m(sizeof(struct sc_rendererUniformData));
    for (sa_u64 i = 0; i < sa_DArray_Length(rendr->bound.uniform_data_array); ++i) {
        sa_DArray_Get(rendr->bound.uniform_data_array, i, uniform_data);
        if (uniform_data->location == uniform_id) {
            sa_Free_m(uniform_data);
            sa_DArray_Set(rendr->bound.uniform_data_array, i, &new_uniform);
            return;
        }
    }
    sa_DArray_Push(rendr->bound.uniform_data_array, &new_uniform);
    sa_Free_m(uniform_data);
}

SA_API void sc_Renderer_Bind_Index_Buffer(struct sc_renderer* rendr,
                                          const sa_dArray* new_indices) {
    if (!new_indices) {
        sa_Log_Warn_Print_m(sa_LOG_SEVERITY_MEDIUM,
                            sa_LOG_CONTEXT_RENDERER,
                            "Indices are NULL and cannot be bound");
        return;
    }

    sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS, sa_LOG_CONTEXT_RENDERER,
                          "Bound %lu indices", sa_DArray_Length(new_indices));
    sa_DArray_Append(rendr->bound.index_array, new_indices);
}

SA_API void sc_Renderer_Push_Mesh_Dynamic(struct sc_renderer* rendr,
                                          const sa_dArray* pos_array,
                                          const sa_dArray* uv_array,
                                          const sa_dArray* color_array) {
#if 0
    s_Renderer_Validate_Before_Push(rendr, pos_array, uv_array, color_array);

    if (sa_DArray_Length(pos_array) > SC_RENDERER_MAX_DYNAMIC_VERT_PER_PUSH) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER,
                              "Trying to push %lu while max per static batch is %d",
                              sa_DArray_Length(pos_array),
                              SC_RENDERER_MAX_DYNAMIC_VERT_PER_PUSH);
        return;
    }

    sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          sa_LOG_CONTEXT_RENDERER,
                          "Attempting to push %lu vertices to static batch",
                          sa_DArray_Length(pos_array));
    sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          sa_LOG_CONTEXT_RENDERER,
                          "Attempting to push %lu indices to static batch",
                          sa_DArray_Length(rendr->bound_info.index_array));

    sa_u32 total_vertices_pushed = 0;
    sa_u32 total_indices_pushed = 0;

    sa_u8 batch_iter = 0;
    while (total_vertices_pushed != sa_DArray_Length(pos_array) ||
           total_indices_pushed != sa_DArray_Length(rendr->bound_info.index_array)) {
        rendr->dynamic_batch_array[batch_iter].texture = 0; // TODO this needs to be removed
        if (!s_Renderer_Batch_Can_Push(rendr, &rendr->dynamic_batch_array[batch_iter])) {
            ++batch_iter;
            continue;
        }
        if (rendr->dynamic_batch_info.in_use >= rendr->dynamic_batch_info.batch_length_max) {
            sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_CALL,
                                 sa_LOG_CONTEXT_RENDERER,
                                 "Vertices pushed filled all calls");
            sc_Renderer_End(rendr);
            s_Renderer_Reset_Batch(rendr);
            batch_iter = 0;
        }

        struct sc_dynamicBatch batch;
        sa_DArray_Get(rendr->dynamic_batch_array, batch_iter, &batch);

        { // Index
            sa_u32 indices_copied =
                s_Renderer_Index_Buffer_Append_Minus_Offset(batch.index_array,
                                                            rendr->bound_index_array_buffer,
                                                            batch->index_array_length,
                                                            total_indices_pushed,
                                                            rendr->batch_index_capacity,
                                                            rendr->bound_index_array_length,
                                                            total_vertices_pushed);
            batch->index_array_length = indices_copied;
            total_indices_pushed += indices_copied;
        }

        { // Vertex
            sa_u32 vertices_copied =
                s_Renderer_Vertex_Array_Append(&batch->vertex_array,
                                               pos_array,
                                               uv_array,
                                               color_array,
                                               total_vertices_pushed);
            batch->vertex_array_length += vertices_copied;
            total_vertices_pushed += vertices_copied;
        }

        batch->texture = rendr->bound_texture_id;
        for (sa_u32 i = 0; i < rendr->bound_uniform_data_capacity; ++i) {
            if (sa_Scast_To_m(int)(rendr->bound_uniform_data[i].type) != 0) {
                rendr->bound_uniform_data[i].type = rendr->bound_uniform_data[i].type;
                rendr->bound_uniform_data[i].value =
                    sa_Calloc_m(sa_Size_Of_Type(rendr->bound_uniform_data[i].type), 1);
                rendr->bound_uniform_data[i].value = rendr->bound_uniform_data[i].value;
            }
        }
    }
    if (rendr->dynamic_batch_info.in_use <= batch_iter) {
        rendr->dynamic_batch_info.in_use = batch_iter + 1;
    }
#endif
}

SA_API void sc_Renderer_Push_Mesh_Instanced(struct sc_renderer* rendr,
                                            const sa_dArray* pos_array,
                                            const sa_dArray* uv_array,
                                            const sa_dArray* color_array,
                                            const sa_dArray* instance_transform_array) {

    s_Renderer_Validate_Before_Push(rendr, pos_array, uv_array, color_array);
    // TODO validate instance_transform

    sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          sa_LOG_CONTEXT_RENDERER,
                          "Attempting to push %lu vertices to instance batch",
                          sa_DArray_Length(pos_array));
    sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          sa_LOG_CONTEXT_RENDERER,
                          "Attempting to push %lu indices to instance batch",
                          sa_DArray_Length(rendr->bound.index_array));
    struct sc_instanceBatch* batch = rendr->instance_batch_address_array[rendr->instance_batch_info.in_use];
    sa_color default_color = {0, 0, 0, 0};
    sa_uv default_uv = {0, 0};
    for (sa_u64 i = 0; i < sa_DArray_Length(pos_array); ++i) {
        sa_vec3 pos;
        sa_color color = default_color;
        sa_uv uv = default_uv;
        sa_DArray_Get(pos_array, i, &pos);
        if (uv_array) {
            sa_DArray_Get(uv_array, i, &uv);
        }
        if (color_array) {
            sa_DArray_Get(color_array, i, &color);
        }
        struct sc_vertex vertex = (struct sc_vertex){pos, color, uv};
        sa_DArray_Push(batch->vertex_array, &vertex);
    }
    sa_DArray_Append(batch->index_array, rendr->bound.index_array);
    sa_DArray_Append(batch->uniform_data, rendr->bound.uniform_data_array);
    sa_DArray_Append(batch->model_matrix_array, instance_transform_array);
    rendr->instance_batch_info.in_use++;
}

#if 0
SA_API void sc_Renderer_Push_Model_Mesh(struct sc_renderer* rendr, const sc_modelMesh* model_mesh) {
}
#endif

SA_API void sc_Renderer_End(struct sc_renderer* rendr) {
#ifndef SACI_RENDERING_DISABLED
    // s_Renderer_Draw_Dynamic_Batch(rendr);
    s_Renderer_Draw_Instance_Batch(rendr);
#endif // SACI_RENDERING_DISABLED
}

SA_API void sc_Renderer_Free(struct sc_renderer* rendr) {
    sc_Renderer_Begin(rendr);
    s_Renderer_Free_Memory(rendr);
    s_Renderer_Free_Opengl(rendr);
    sa_Free_m(rendr);
    rendr = NULL;
}

SA_API void sc_Renderer_Free_Opts(struct sc_renderer* rendr, int free_opts) {
    sc_Renderer_Begin(rendr);
    if (free_opts & sc_RENDERER_FREE_OPT_MEMORY) {
        s_Renderer_Free_Memory(rendr);
    }
    if (free_opts & sc_RENDERER_FREE_OPT_OPENGL) {
        s_Renderer_Free_Opengl(rendr);
    }
    sa_Free_m(rendr);
    rendr = NULL;
}

SA_API sa_s32 sc_Renderer_Get_Uniform_Id(struct sc_renderer* rendr,
                                         const char* const uniform_name) {
    return sc_GL_Uniform_Location(rendr->shader_program, uniform_name);
}

/* --- Renderer Helper impl --- */

SA_INTERNAL_INLINE void s_Renderer_Free_Opengl(struct sc_renderer* rendr) {
#ifndef SACI_RENDERING_DISABLED
    glDeleteBuffers(1, &(rendr)->ibo);
    glDeleteBuffers(1, &(rendr)->vbo);
    glDeleteBuffers(1, &(rendr)->vbo);
    glDeleteVertexArrays(1, &(rendr)->vao);
    glDeleteProgram((rendr)->shader_program);
#endif // SACI_RENDERING_DISABLED
}

SA_INTERNAL void s_Renderer_Free_Batch_Array(struct sc_renderer* rendr) {
#if 0
    if (rendr->dynamic_batch_arena) {
        for (sa_u8 i = 0; i < rendr->batch_array_capacity; ++i) {
            if (rendr->dynamic_batch_array[i].index_array) {
                sa_Free_m(rendr->dynamic_batch_array[i].index_array);
                rendr->dynamic_batch_array[i].index_array = NULL;
            }
            sc_Vertex_Array_Free(&rendr->dynamic_batch_array[i].vertex_array);
            if (rendr->dynamic_batch_array[i].bound_uniform_data) {
                for (sa_u32 j = 0; j < rendr->bound_uniform_data_capacity; ++j) {
                    if (rendr->dynamic_batch_array[i].bound_uniform_data[j].value) {
                        sa_Free_m(rendr->dynamic_batch_array[i].bound_uniform_data[j].value);
                        rendr->dynamic_batch_array[i].bound_uniform_data[j].value = NULL;
                    }
                }
            }
        }
        sa_Free_m(rendr->dynamic_batch_array);
        rendr->dynamic_batch_array = NULL;
    }
#endif
}

SA_INTERNAL void s_Renderer_Free_Memory(struct sc_renderer* rendr) {
    if (!rendr) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM,
                             sa_LOG_CONTEXT_RENDERER,
                             "Trying to free a NULL renderer");
        return;
    }
    sa_DArray_Free(rendr->bound.uniform_data_array);
    sa_DArray_Free(rendr->bound.index_array);
    sa_Free_m(sa_Scast_To_m(void*)(rendr->instance_batch_address_array));
    ArenaDelete(&rendr->instance_batch_arena);
    s_Renderer_Free_Batch_Array(rendr);
}

SA_INTERNAL void s_Renderer_Reset_Bound(struct sc_renderer* rendr) {
    sa_DArray_Clear(rendr->bound.uniform_data_array);
    sa_DArray_Clear(rendr->bound.index_array);
    rendr->bound.texture = SC_TEXTURE_INVALID;
}

SA_INTERNAL void s_Renderer_Reset_Batch(struct sc_renderer* rendr) {
    sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                         sa_LOG_CONTEXT_RENDERER,
                         "Reseting batches");
    rendr->instance_batch_info.in_use = 0;
    for (sa_u8 i = 0; i < rendr->instance_batch_info.batch_length_max; ++i) {
        struct sc_instanceBatch* batch = rendr->instance_batch_address_array[i];
        batch->texture = SC_TEXTURE_INVALID;
        sa_DArray_Clear(batch->index_array);
        sa_DArray_Clear(batch->model_matrix_array);
        sa_DArray_Clear(batch->vertex_array);
        sa_DArray_Clear(batch->uniform_data);
    }
}

SA_INTERNAL sa_bool s_Renderer_Validate_Before_Push(const struct sc_renderer* rendr,
                                                    const sa_dArray* pos_array,
                                                    const sa_dArray* uv_array,
                                                    const sa_dArray* color_array) {
    if (!pos_array) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER,
                             "Vertex array being pushed has length ZERO or is NULL");
        return sa_FALSE;
    }
    if (sa_DArray_Is_Null(pos_array) || !sa_DArray_Length(pos_array)) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER,
                             "Vertex array being pushed has length ZERO or is NULL");
        return sa_FALSE;
    }
    if (sa_DArray_Is_Null(rendr->bound.index_array) || !sa_DArray_Length(rendr->bound.index_array)) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER, "NULL index array bound");
        return sa_FALSE;
    }

    sa_Log_Debug_Condition_Print_m(uv_array, sa_LOG_DEBUG_TYPE_RENDERER,
                                   sa_LOG_CONTEXT_OPENGL, "NULL uv array param");

    sa_Log_Debug_Condition_Print_m(color_array, sa_LOG_DEBUG_TYPE_RENDERER,
                                   sa_LOG_CONTEXT_OPENGL, "NULL color array param");

    return sa_TRUE;
}

SA_INTERNAL void s_Renderer_Initialize_Config(struct sc_rendererConfig* rendr_cfg, const char* cfg_path) {
    sc_configState* cfg_state = sc_Config_Load(
        cfg_path);

    // Default values
    rendr_cfg->bound_info.index_array_max_length = SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;

    rendr_cfg->static_batch_info.index_array_max = SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY;
    rendr_cfg->instance_batch_info.index_array_max = SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY;

    rendr_cfg->static_batch_info.vertex_array_max = SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY;
    rendr_cfg->instance_batch_info.vertex_array_max = SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY;

    rendr_cfg->static_batch_info.batch_length_max = SC_RENDERER_DEFAULT_BATCH_CAPACITY;
    rendr_cfg->instance_batch_info.batch_length_max = SC_RENDERER_DEFAULT_BATCH_CAPACITY;
    rendr_cfg->bound_info.uniform_array_max_length = 1024;
    rendr_cfg->instance_batch_info.transform_array_max = 1024; // TODO

    if (!sc_Config_Load_Table(cfg_state, "core_renderer")) {
        return;
    }
    rendr_cfg->bound_info.index_array_max_length =
        sc_Config_Get_Int64(cfg_state, "bound_index_array_capacity");

    rendr_cfg->instance_batch_info.index_array_max =
        sc_Config_Get_Int64(cfg_state, "batch_index_capacity");
    rendr_cfg->static_batch_info.index_array_max =
        sc_Config_Get_Int64(cfg_state, "batch_index_capacity");

    rendr_cfg->instance_batch_info.batch_length_max =
        sc_Config_Get_Int8(cfg_state, "batch_array_capacity");
    rendr_cfg->static_batch_info.batch_length_max =
        sc_Config_Get_Int8(cfg_state, "batch_array_capacity");

    rendr_cfg->static_batch_info.vertex_array_max =
        sc_Config_Get_Int32(cfg_state, "batch_vertex_capacity");
    rendr_cfg->instance_batch_info.vertex_array_max =
        sc_Config_Get_Int32(cfg_state, "batch_vertex_capacity");

    const char* vert_shader = sc_Config_Get_Str(cfg_state, "vert_shader");
    if (vert_shader) {
        strncpy(rendr_cfg->v_shader_code, vert_shader, strlen(vert_shader));
    }
    const char* frag_shader = sc_Config_Get_Str(cfg_state, "frag_shader");
    if (frag_shader) {
        strncpy(rendr_cfg->f_shader_code, frag_shader, strlen(frag_shader));
    }
    sc_Config_Close(cfg_state);
}

SA_INTERNAL void s_Renderer_Init_GL(struct sc_renderer* rendr, const struct sc_rendererConfig rendr_cfg) {
#ifndef SACI_RENDERING_DISABLED
    { // Shader init
        sa_shaderId v_shader = sc_Shader_Compile_Shader_Vert(sc_VERT_SHADER);
        sa_shaderId f_shader = sc_Shader_Compile_Shader_Frag(sc_FRAG_SHADER);
        sa_Log_Assert_Message_m(v_shader && f_shader, "Shaders could not be initialized");
        rendr->shader_program = sc_Shader_Create_Shader_Program(v_shader, f_shader);
        sa_Log_Assert_Message_m(rendr->shader_program, "Shader program could not be initialized");
    }
    { // Opengl buffers
        rendr->vbo = sc_GL_Create_Vertex_Buffer(
            sizeof(struct sc_vertex) * SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY,
            NULL, GL_DYNAMIC_DRAW);
        sa_Log_Assert_Message_m(rendr->vbo, "VBO could not be initialized");
        rendr->ibo = sc_GL_Create_Index_Buffer_Dynamic(NULL, SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY);
        sa_Log_Assert_Message_m(rendr->ibo, "IBO could not be initialized");
        sc_GL_Create_Vertex_Array(1, &rendr->vao);
        sa_Log_Assert_Message_m(rendr->vao, "VAO could not be initialized");
        // TODO
        rendr->instance_vbo = sc_GL_Create_Vertex_Buffer(
            sizeof(sa_mat4) * 1024, NULL, GL_DYNAMIC_DRAW);
    }
    { // VertexAttrib init
        sc_GL_Bind_Vertex_Array(rendr->vao);

        sc_GL_Bind_Vertex_Buffer(rendr->vbo);

        sc_GL_Set_Vertex_Attrib_Pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct sc_vertex),
                                        sa_Scast_To_m(void*) offsetof(struct sc_vertex, pos));
        sc_GL_Enable_Vertex_Attrib_Array(0);
        sc_GL_Set_Vertex_Attrib_Pointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct sc_vertex),
                                        sa_Scast_To_m(void*) offsetof(struct sc_vertex, color));
        sc_GL_Enable_Vertex_Attrib_Array(1);
        sc_GL_Set_Vertex_Attrib_Pointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct sc_vertex),
                                        sa_Scast_To_m(void*) offsetof(struct sc_vertex, uv));
        sc_GL_Enable_Vertex_Attrib_Array(2);

        sc_GL_Bind_Vertex_Buffer(rendr->instance_vbo);
        for (sa_u32 i = 0; i < 4; ++i) {
            sc_GL_Set_Vertex_Attrib_Pointer(3 + i, 4, GL_FLOAT, GL_FALSE,
                                            sizeof(sa_mat4), (void*)(sizeof(float) * i * 4));
            sc_GL_Enable_Vertex_Attrib_Array(3 + i);
            glVertexAttribDivisor(3 + i, 1);
        }
    }
#endif // SACI_RENDERING_DISABLED
}

SA_INTERNAL void s_Renderer_Init_Dynamic_Batch(struct sc_renderer* rendr) {
    struct sc_staticBatchInfo dyn_info = rendr->static_batch_info;
    struct sc_rendererBoundInfo bound_info = rendr->bound_info;
    sa_u64 index_size = dyn_info.index_array_max * sizeof(sa_u32);
    sa_u64 vertex_size = dyn_info.vertex_array_max * sizeof(struct sc_vertex);
    sa_u64 uniform_size = bound_info.uniform_array_max_length * sizeof(struct sc_rendererUniformData);
    sa_u64 batch_arena_element_size = index_size + vertex_size + uniform_size + sizeof(struct sc_staticBatch);
    rendr->static_batch_info.batch_struct_allocation_size = batch_arena_element_size;

    // TODO
    // rendr->static_batch_address_array = sa_DArray_Create(
    //     dyn_info.batch_length_max,
    //     sizeof(struct sc_staticBatch*),
    //     sa_TRUE);

    ArenaInit(
        &rendr->static_batch_arena,
        batch_arena_element_size * dyn_info.batch_length_max + 1024);

    for (sa_u8 i = 0; i < dyn_info.batch_length_max; ++i) {
        void* dyn_batch_mem = ArenaPush(&rendr->static_batch_arena, sizeof(struct sc_staticBatch));
        void* index_mem = ArenaPush(&rendr->static_batch_arena, index_size);
        void* vertex_mem = ArenaPush(&rendr->static_batch_arena, vertex_size);
        void* uniform_mem = ArenaPush(&rendr->static_batch_arena, uniform_size);

        struct sc_staticBatch* dyn_batch =
            sa_Scast_To_m(struct sc_staticBatch*)(dyn_batch_mem);
        dyn_batch->texture = SC_TEXTURE_INVALID;
        dyn_batch->index_array = sa_DArray_Create_Ctx(
            index_mem,
            index_size + SIZE_OF_DARRAY,
            dyn_info.index_array_max,
            sizeof(sa_u32),
            sa_TRUE);
        dyn_batch->vertex_array = sa_DArray_Create_Ctx(
            vertex_mem,
            vertex_size + SIZE_OF_DARRAY,
            dyn_info.vertex_array_max,
            sizeof(struct sc_vertex),
            sa_TRUE);
        dyn_batch->uniform_data = sa_DArray_Create_Ctx(
            uniform_mem,
            uniform_size + SIZE_OF_DARRAY,
            bound_info.uniform_array_max_length,
            sizeof(struct sc_rendererUniformData),
            sa_TRUE);
        sa_DArray_Push(rendr->static_batch_address_array, dyn_batch);
    }
}

SA_INTERNAL void s_Renderer_Init_Instance_Batch(struct sc_renderer* rendr) {
    struct sc_instanceBatchInfo instance_info = rendr->instance_batch_info;
    struct sc_rendererBoundInfo bound_info = rendr->bound_info;
    sa_u64 index_size = instance_info.index_array_max * sizeof(sa_u32);
    sa_u64 vertex_size = instance_info.vertex_array_max * sizeof(struct sc_vertex);
    sa_u64 transform_size = instance_info.transform_array_max * sizeof(sa_mat4);
    sa_u64 uniform_size = bound_info.uniform_array_max_length * sizeof(struct sc_rendererUniformData);
    sa_u64 batch_arena_element_size = index_size + vertex_size + transform_size + uniform_size + sizeof(struct sc_instanceBatch);
    rendr->instance_batch_info.batch_struct_allocation_size = batch_arena_element_size;

    rendr->instance_batch_address_array = sa_Calloc_m(instance_info.batch_length_max, sizeof(struct sc_instanceBatch*));
    ArenaInit(
        &rendr->instance_batch_arena,
        batch_arena_element_size * instance_info.batch_length_max + 1024);

    for (sa_u8 i = 0; i < instance_info.batch_length_max; ++i) {
        void* instance_batch_mem = ArenaPush(&rendr->instance_batch_arena, sizeof(struct sc_instanceBatch));
        void* index_mem = ArenaPush(&rendr->instance_batch_arena, index_size);
        void* vertex_mem = ArenaPush(&rendr->instance_batch_arena, vertex_size);
        void* transform_mem = ArenaPush(&rendr->instance_batch_arena, transform_size);
        void* uniform_mem = ArenaPush(&rendr->instance_batch_arena, uniform_size);

        struct sc_instanceBatch* instance_batch =
            sa_Scast_To_m(struct sc_instanceBatch*)(instance_batch_mem);
        instance_batch->texture = SC_TEXTURE_INVALID;
        instance_batch->index_array = sa_DArray_Create_Ctx(
            index_mem,
            index_size + SIZE_OF_DARRAY,
            instance_info.index_array_max,
            sizeof(sa_u32),
            sa_TRUE);
        instance_batch->vertex_array = sa_DArray_Create_Ctx(
            vertex_mem,
            vertex_size + SIZE_OF_DARRAY,
            instance_info.vertex_array_max,
            sizeof(struct sc_vertex),
            sa_TRUE);
        instance_batch->model_matrix_array = sa_DArray_Create_Ctx(
            transform_mem,
            transform_size + SIZE_OF_DARRAY,
            instance_info.transform_array_max,
            sizeof(sa_mat4),
            sa_TRUE);
        instance_batch->uniform_data = sa_DArray_Create_Ctx(
            uniform_mem,
            uniform_size + SIZE_OF_DARRAY,
            bound_info.uniform_array_max_length,
            sizeof(struct sc_rendererUniformData),
            sa_TRUE);
        rendr->instance_batch_address_array[i] = instance_batch;
    }
}

SA_INTERNAL void s_Renderer_Init_Batch(struct sc_renderer* rendr, const struct sc_rendererConfig rendr_cfg) {
    rendr->static_batch_info = rendr_cfg.static_batch_info;
    rendr->instance_batch_info = rendr_cfg.instance_batch_info;
    rendr->bound_info = rendr_cfg.bound_info;
    rendr->instance_batch_info.in_use = 0;
    rendr->static_batch_info.in_use = 0;

    s_Renderer_Init_Dynamic_Batch(rendr);
    s_Renderer_Init_Instance_Batch(rendr);

    rendr->bound.index_array = sa_DArray_Create(
        rendr_cfg.bound_info.index_array_max_length,
        sizeof(sa_u32),
        sa_TRUE);

    rendr->bound.uniform_data_array = sa_DArray_Create(
        rendr_cfg.bound_info.uniform_array_max_length,
        sizeof(struct sc_rendererUniformData),
        sa_FALSE);
}

SA_INTERNAL sa_u32 s_Renderer_Index_Buffer_Append_Minus_Offset(sa_u32* dest_out,
                                                               sa_u32* src,
                                                               sa_u32 dest_indices_pushed,
                                                               sa_u32 src_indices_pushed,
                                                               sa_u32 dest_capacity,
                                                               sa_u32 src_count,
                                                               sa_u32 offset) {
    sa_Log_Assert_Message_m(dest_out, "Invalid pointer for index array");
    sa_u32 indices_copied_out = 0;

    if (dest_indices_pushed >= dest_capacity) {
        sa_Log_Assert_Message_m(dest_out, "Destination buffer overflow");
        return indices_copied_out;
    }
    if (src_indices_pushed >= src_count) {
        sa_Log_Assert_Message_m(src, "Source buffer overflow");
        return indices_copied_out;
    }

    sa_u32 remaining_indices = src_count - src_indices_pushed;
    sa_u32 indices_to_copy = sa_Min_m(dest_capacity - dest_indices_pushed, remaining_indices);
    for (sa_u64 i = 0; i < indices_to_copy; ++i) {
        dest_out[dest_indices_pushed + i] = (src[src_indices_pushed + i] - offset);
    }
    indices_copied_out = indices_to_copy;
    return indices_copied_out;
}

SA_INTERNAL sa_bool s_Renderer_Batch_Can_Push(const sc_renderer* rendr, const struct sc_staticBatch* batch) {
#if 0
    if (batch->vertex_array_length >= rendr->batch_vertex_capacity) {
        sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
                             "Batch has reached it's vertex limit, going to next one");
        return sa_FALSE;
    }
    if (batch->index_array_length >= rendr->bound_index_array_capacity) {
        sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
                             "Batch has reached it's index limit, going to next one");
        return sa_FALSE;
    }

    if (batch->texture != rendr->bound_texture_id && batch->texture != 0) {
        sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
                              "Batch has different texture (%d) than bound (%d), skipping to next batch",
                              batch->texture, rendr->bound_texture_id);
        return sa_FALSE;
    }
    // TODO
    // if (batch->uniform_struct_block_size != rendr->bound_uniform_struct_size &&
    //     batch->uniform_struct_block_size != 0) {
    //     sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
    //                          "Batch uniform block SIZE is different than bound, skipping to next batch");
    //     return sa_FALSE;
    // }
    // if (batch->uniform_struct_block_size != 0 &&
    //     s_Renderer_Uniform_Is_Equal(batch->uniform_struct_block, batch->uniform_struct_block, batch->uniform_struct_block_size)) {
    //     sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
    //                          "Batch uniform is different than bound, skipping to next batch");
    //     return sa_FALSE;
    // }
#endif
    return sa_TRUE;
}

SA_INTERNAL union sc_uniformValue s_Renderer_Uniform_Value_From_Type(sa_dataType type, const void* value) {
    union sc_uniformValue result = {0};

    switch (type) {
    case SA_TYPE_U8:
        result.u8 = *(const sa_u8*)value;
        break;
    case SA_TYPE_U16:
        result.u16 = *(const sa_u16*)value;
        break;
    case SA_TYPE_U32:
        result.u32 = *(const sa_u32*)value;
        break;
    case SA_TYPE_U64:
        result.u64 = *(const sa_u64*)value;
        break;

    case SA_TYPE_S8:
        result.s8 = *(const sa_s8*)value;
        break;
    case SA_TYPE_S16:
        result.s16 = *(const sa_s16*)value;
        break;
    case SA_TYPE_S32:
        result.s32 = *(const sa_s32*)value;
        break;
    case SA_TYPE_S64:
        result.s64 = *(const sa_s64*)value;
        break;

    case SA_TYPE_BOOL:
        result.boolean = *(const sa_bool*)value;
        break;

    case SA_TYPE_UV:
        result.uv = *(const sa_uv*)value;
        break;
    case SA_TYPE_VEC2:
        result.vec2 = *(const sa_vec2*)value;
        break;
    case SA_TYPE_VEC3:
        result.vec3 = *(const sa_vec3*)value;
        break;
    case SA_TYPE_VEC4:
        result.vec4 = *(const sa_vec4*)value;
        break;

    case SA_TYPE_COLOR:
        result.color = *(const sa_color*)value;
        break;

    case SA_TYPE_MAT2:
    case SA_TYPE_MAT3:
        break;
    case SA_TYPE_MAT4:
        result.mat4 = *(const sa_mat4*)value;
        break;

    case SA_TYPE_MAT2X3:
    case SA_TYPE_MAT2X4:
    case SA_TYPE_MAT3X2:
    case SA_TYPE_MAT3X4:
    case SA_TYPE_MAT4X2:
    case SA_TYPE_MAT4X3:
        break;

    default:
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_OPENGL,
                              "Invalid type %d for uniform", type);
        break;
    }

    return result;
}

SA_INTERNAL void s_Renderer_Set_Uniform_From_Uniform_Data(const struct sc_rendererUniformData uniform_data) {
    const void* value_ptr = NULL;

    switch (uniform_data.type) {
    case SA_TYPE_U8:
        value_ptr = &uniform_data.value.u8;
        break;
    case SA_TYPE_U16:
        value_ptr = &uniform_data.value.u16;
        break;
    case SA_TYPE_U32:
    case SA_TYPE_SHADERID:
    case SA_TYPE_TEXTUREID:
    case SA_TYPE_BUFFERID:
        value_ptr = &uniform_data.value.u32;
        break;
    case SA_TYPE_U64:
        value_ptr = &uniform_data.value.u64;
        break;

    case SA_TYPE_S8:
        value_ptr = &uniform_data.value.s8;
        break;
    case SA_TYPE_S16:
        value_ptr = &uniform_data.value.s16;
        break;
    case SA_TYPE_S32:
        value_ptr = &uniform_data.value.s32;
        break;
    case SA_TYPE_S64:
        value_ptr = &uniform_data.value.s64;
        break;

    case SA_TYPE_BOOL:
        value_ptr = &uniform_data.value.boolean;
        break;

    case SA_TYPE_UV:
        value_ptr = &uniform_data.value.uv;
        break;
    case SA_TYPE_VEC2:
        value_ptr = &uniform_data.value.vec2;
        break;
    case SA_TYPE_VEC3:
        value_ptr = &uniform_data.value.vec3;
        break;
    case SA_TYPE_VEC4:
        value_ptr = &uniform_data.value.vec4;
        break;

    case SA_TYPE_COLOR:
        value_ptr = &uniform_data.value.color;
        break;

    case SA_TYPE_MAT4:
        value_ptr = &uniform_data.value.mat4;
        break;

    // TODO NEED TO BE ADDED
    case SA_TYPE_MAT2:
    case SA_TYPE_MAT3:
    case SA_TYPE_MAT2X3:
    case SA_TYPE_MAT2X4:
    case SA_TYPE_MAT3X2:
    case SA_TYPE_MAT3X4:
    case SA_TYPE_MAT4X2:
    case SA_TYPE_MAT4X3:
        break;

    default:
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_OPENGL,
                              "Invalid type %d for uniform", uniform_data.type);
        return;
    }

    sc_GL_Uniform_Set_Value(uniform_data.location, uniform_data.type, value_ptr);
}

SA_INTERNAL void s_Renderer_Draw_Dynamic_Batch(const sc_renderer* rendr) {
#if 0
    sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS, sa_LOG_CONTEXT_RENDERER, "Flushing %d batches", rendr->batch_in_use);
    struct previousBatch { // Will get bigger later
        sa_textureId texture;
    } previous_batch = {0};
    for (sa_u8 i = 0; i < rendr->batch_in_use; ++i) {
        struct sc_dynamicBatch* batch = &rendr->dynamic_batch_array[i];

        glUseProgram(rendr->shader_program);

        glBindVertexArray(rendr->vao);

        glBindBuffer(GL_ARRAY_BUFFER, rendr->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        sa_Scast_To_m(long int)(sizeof(struct sc_vertex) * batch->vertex_array_length),
                        batch->vertex_array.data);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendr->ibo);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                        sa_Scast_To_m(long int)(sizeof(sa_u32) * batch->index_array_length),
                        batch->index_array);

        { // Uniforms
            {
                // TODO should be removed
                glEnable(GL_DEPTH_TEST);
            }
            for (sa_u32 j = 0; j < rendr->bound_uniform_data_capacity; ++j) {
                if (batch->bound_uniform_data[i].type != 0) {
                    sc_GL_Uniform_Set_Value(i, batch->bound_uniform_data[i].type,
                                            batch->bound_uniform_data[i].value);
                }
            }
        }

        if (batch->texture != previous_batch.texture) {
            glUniform1i(SC_U_USE_TEXTURE_LOC, sa_TRUE);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, batch->texture);
            previous_batch.texture = batch->texture;
        }

        glDrawElements(GL_TRIANGLES, sa_Scast_To_m(int)(batch->index_array_length), GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
#endif
}

SA_INTERNAL void s_Renderer_Draw_Instance_Batch(const sc_renderer* rendr) {
    sa_Log_DebugF_Print_m(
        sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
        sa_LOG_CONTEXT_RENDERER,
        "Flushing %d instance batches",
        rendr->instance_batch_info.in_use);

    struct previousBatch { // Will get bigger later
        sa_textureId texture;
    } previous_batch = {0};

    for (sa_u8 i = 0; i < rendr->instance_batch_info.in_use; ++i) {
        struct sc_instanceBatch* batch = rendr->instance_batch_address_array[i];

        glUseProgram(rendr->shader_program);

        glBindVertexArray(rendr->vao);

        glBindBuffer(GL_ARRAY_BUFFER, rendr->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        sa_Scast_To_m(long int)(sizeof(struct sc_vertex) * sa_DArray_Length(batch->vertex_array)),
                        sa_DArray_Get_Ptr(batch->vertex_array, 0));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendr->ibo);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                        sa_Scast_To_m(long int)(sizeof(sa_u32) * sa_DArray_Length(batch->index_array)),
                        sa_DArray_Get_Ptr(batch->index_array, 0));

        glBindBuffer(GL_ARRAY_BUFFER, rendr->instance_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        sa_Scast_To_m(long int)(sizeof(sa_mat4) * sa_DArray_Length(batch->model_matrix_array)),
                        sa_DArray_Get_Ptr(batch->model_matrix_array, 0));

        { // Uniforms
            {
                // TODO should be removed
                glEnable(GL_DEPTH_TEST);
            }
            for (sa_u32 j = 0; j < sa_DArray_Length(rendr->bound.uniform_data_array); ++j) {
                struct sc_rendererUniformData uniform_data = {0};
                sa_DArray_Get(batch->uniform_data, j, &uniform_data);
                s_Renderer_Set_Uniform_From_Uniform_Data(uniform_data);
            }
        }

        if (batch->texture != previous_batch.texture && batch->texture != SC_TEXTURE_INVALID) {
            glUniform1i(SC_U_USE_TEXTURE_LOC, sa_TRUE);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, batch->texture);
            previous_batch.texture = batch->texture;
        }

        glDrawElementsInstanced(GL_TRIANGLES,
                                sa_Scast_To_m(int)(sa_DArray_Length(batch->index_array)),
                                GL_UNSIGNED_INT,
                                0,
                                sa_Scast_To_m(int)(sa_DArray_Length(batch->model_matrix_array))); // instance count
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}
