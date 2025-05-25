// TODO uniform_struct_block needs to have an option to be freed, but also need
// to evaludate if it will free the user's uniform_struct_block. Perhaps a copy
// would be nice
#include <glad/glad.h>

#include "saci-core/sc-gl.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-darray.h"
#include "saci-utils/su-darray-internal.h"
#include "saci-core/sc-config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#ifndef SC_RENDERER_STRUCT
#  define SC_RENDERER_STRUCT

typedef struct sc_vertex {
    sa_vec3 pos;
    sa_color color;
    sa_uv uv;
} sc_vertex;

sa_Create_Darray_m(sc_Vertex_Array, sc_vertex);

// The fields are structured in a way that enforces minimum memory change over time
struct sc_renderer {
    sa_textureId bound_texture_id;
    sa_u32 bound_index_array_length;
    sa_u32 bound_index_array_capacity;

    sa_shaderId shader_program;
    sa_bufferId ibo, vbo, vao;
    sa_bufferId instance_vbo; // used for instancing

    sa_u32 batch_index_capacity;
    sa_u32 batch_vertex_capacity;
    sa_u8 batch_array_capacity;
    sa_u8 batch_in_use;

    sa_u32 bound_uniform_data_capacity;
    struct sc_uniformData {
        void* value;
        sa_dataType type;
    }* bound_uniform_data; // each index represents a location

    struct sc_instanceBatch {
        sa_u32 instance_count;
        sa_textureId texture;
        sa_u32 index_array_length;
        sa_u32 vertex_array_length;
        sa_mat4* model_matrix_array; // count is instance_count
        sa_u32* index_array;
        struct sc_vertex* vertex_array;
        struct sc_uniformData* bound_uniform_data;
    }* instance_batch;

    struct sc_dynamicBatch {
        sa_textureId texture;
        sa_u32 index_array_length;
        sa_u32 vertex_array_length;
        sa_u32* index_array;
        sc_vertexArray* vertex_array;
        struct sc_uniformData* bound_uniform_data;
    }* dynamic_batch_array;

    sa_u32* bound_index_array_buffer;
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
                                                    const sa_vec3Array* pos_array,
                                                    const sa_uvArray* uv_array,
                                                    const sa_colorArray* color_array);

SA_INTERNAL void s_Renderer_Initialize_Config(struct sc_renderer* rendr, const char* cfg_path);

// Initializes with default opengl data
SA_INTERNAL void s_Renderer_Init_GL(struct sc_renderer* rendr);

// Initializes with default batch data
SA_INTERNAL void s_Renderer_Init_Batch(struct sc_renderer* rendr);

// Compares two uniforms and returns if they are equal
SA_INTERNAL sa_bool s_Renderer_Uniform_Is_Equal(sa_u8* u1, sa_u8* u2, sa_u64 size);

// Appends an index buffer (dest)
SA_INTERNAL sa_u32 s_Renderer_Index_Buffer_Append_Minus_Offset(sa_u32* dest_out,
                                                               sa_u32* src,
                                                               sa_u32 dest_indices_pushed,
                                                               sa_u32 src_indices_pushed,
                                                               sa_u32 dest_capacity,
                                                               sa_u32 src_count,
                                                               sa_u32 offset);

SA_INTERNAL sa_u32 s_Renderer_Vertex_Array_Append(sc_vertexArray* dest_out,
                                                  const sa_vec3Array* src_pos,
                                                  const sa_uvArray* src_uv,
                                                  const sa_colorArray* src_color,
                                                  sa_u32 src_count);

SA_INTERNAL sa_bool s_Renderer_Batch_Can_Push(const sc_renderer* rendr, const struct sc_dynamicBatch* batch);

SA_INTERNAL void s_Renderer_Draw_Dynamic_Batch(const sc_renderer* rendr);

SA_INTERNAL void s_Renderer_Draw_Dynamic_Batch(const sc_renderer* rendr);

/* --- Renderer Header Impl --- */

SA_API sc_renderer* sc_Renderer_New_Default(void) {
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
    struct sc_renderer* rendr = sa_Calloc_m(1, sizeof(struct sc_renderer));
    sa_Log_Assert_Message_m(rendr, "Renderer could not be created");
    s_Renderer_Initialize_Config(rendr, file_path);
    s_Renderer_Init_GL(rendr);
    s_Renderer_Init_Batch(rendr);

    { // Bound info
        rendr->bound_index_array_capacity = SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;
        rendr->bound_index_array_buffer =
            sa_Calloc_m(rendr->bound_index_array_capacity, sizeof(sa_u32));
    }

    return rendr;
}

SA_API void sc_Renderer_Begin(struct sc_renderer* rendr) {
    s_Renderer_Reset_Batch(rendr);
    s_Renderer_Reset_Bound(rendr);
}

SA_API void sc_Renderer_Bind_Texture(struct sc_renderer* rendr, sa_textureId tex_id) {
    rendr->bound_texture_id = tex_id;
}

SA_API void sc_Renderer_Set_Uniform(struct sc_renderer* rendr,
                                    const sa_u32 uniform_id,
                                    const void* const value,
                                    const sa_dataType type) {
    if (uniform_id == 0) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_RENDERER,
                             "Trying to bind uniform with id zero");
        return;
    }
    if (sa_Scast_To_m(int)(type) == 0 || sa_Scast_To_m(int)(type) > 26) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_RENDERER,
                             "Trying to bind uniform with invalid type");
        return;
    }
    rendr->bound_uniform_data[uniform_id - 1].type = type;
    sa_u64 size_of_type = sa_Size_Of_Type(type);
    rendr->bound_uniform_data[uniform_id - 1].value = sa_Malloc_m(size_of_type);
    memcpy(rendr->bound_uniform_data[uniform_id - 1].value, value, size_of_type);
}

SA_API void sc_Renderer_Bind_Index_Buffer(struct sc_renderer* rendr,
                                          const sa_u32Array* new_indices) {
    // Reset the index buffer
    if (!new_indices) {
        sa_Log_Warn_Print_m(sa_LOG_SEVERITY_LOW,
                            sa_LOG_CONTEXT_RENDERER,
                            "Indices are NULL and cannot be bound");
        return;
    }

    sa_u32 new_indices_count = sa_Scast_To_m(sa_u32)(new_indices->length);
    if (new_indices_count > rendr->bound_index_array_capacity) {
        sa_Log_WarnF_Print_m(sa_LOG_SEVERITY_MEDIUM,
                             sa_LOG_CONTEXT_RENDERER,
                             "Indice bound of capacity: %d is larger than max capacity: %d",
                             new_indices_count, rendr->bound_index_array_capacity);
        return;
    }

    sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS, sa_LOG_CONTEXT_RENDERER,
                          "Bound %d indices", new_indices_count);
    memcpy(rendr->bound_index_array_buffer, new_indices->data, new_indices_count * sizeof(sa_u32));
    rendr->bound_index_array_length = new_indices_count;
}

SA_API void sc_Renderer_Push_Mesh_Dynamic(struct sc_renderer* rendr,
                                          const sa_vec3Array* pos_array,
                                          const sa_uvArray* uv_array,
                                          const sa_colorArray* color_array) {
    s_Renderer_Validate_Before_Push(rendr, pos_array, uv_array, color_array);

    if (pos_array->length > SC_RENDERER_MAX_DYNAMIC_VERT_PER_PUSH) {
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER,
                              "Trying to push %lu while max per dynamic batch is %d",
                              pos_array->length,
                              SC_RENDERER_MAX_DYNAMIC_VERT_PER_PUSH);
        return;
    }

    sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          sa_LOG_CONTEXT_RENDERER,
                          "Attempting to push %lu vertices to dynamic batch",
                          pos_array->length);
    sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          sa_LOG_CONTEXT_RENDERER,
                          "Attempting to push %d indices to dynamic batch",
                          rendr->bound_index_array_length);

    sa_u32 total_vertices_pushed = 0;
    sa_u32 total_indices_pushed = 0;

    sa_u8 batch_iter = 0;
    while (total_vertices_pushed != pos_array->length ||
           total_indices_pushed != rendr->bound_index_array_length) {

        rendr->dynamic_batch_array[batch_iter].texture = 0; // TODO this needs to be removed
        if (!s_Renderer_Batch_Can_Push(rendr, &rendr->dynamic_batch_array[batch_iter])) {
            ++batch_iter;
            continue;
        }
        if (rendr->batch_in_use >= rendr->batch_array_capacity) {
            sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_CALL,
                                 sa_LOG_CONTEXT_RENDERER,
                                 "Vertices pushed filled all calls");
            sc_Renderer_End(rendr);
            s_Renderer_Reset_Batch(rendr);
            batch_iter = 0;
        }

        struct sc_dynamicBatch* batch = &(rendr->dynamic_batch_array[batch_iter]);
        sa_Log_Assert_Message_m(batch, "Could not point to call in use");

        { // Index
            sa_u32 indices_copied =
                s_Renderer_Index_Buffer_Append_Minus_Offset(batch->index_array,
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
                s_Renderer_Vertex_Array_Append(batch->vertex_array, pos_array,
                                               uv_array, color_array, total_vertices_pushed);
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
    if (rendr->batch_in_use <= batch_iter) {
        rendr->batch_in_use = batch_iter + 1;
    }
}

#if 0
SA_API void sc_Renderer_Push_Model_Mesh(struct sc_renderer* rendr, const sc_modelMesh* model_mesh) {
}
#endif

SA_API void sc_Renderer_End(struct sc_renderer* rendr) {
#ifndef SACI_RENDERING_DISABLED
    s_Renderer_Draw_Dynamic_Batch(rendr);
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

SA_API sa_u32 sc_Renderer_Get_Uniform_Id(struct sc_renderer* rendr,
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
    if (rendr->dynamic_batch_array) {

        for (sa_u8 i = 0; i < rendr->batch_array_capacity; ++i) {
            if (rendr->dynamic_batch_array[i].index_array) {
                sa_Free_m(rendr->dynamic_batch_array[i].index_array);
                rendr->dynamic_batch_array[i].index_array = NULL;
            }
            if (rendr->dynamic_batch_array[i].vertex_array) {
                sa_Free_m(rendr->dynamic_batch_array[i].vertex_array);
                rendr->dynamic_batch_array[i].vertex_array = NULL;
            }
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
}

SA_INTERNAL void s_Renderer_Free_Memory(struct sc_renderer* rendr) {
    if (!rendr) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM,
                             sa_LOG_CONTEXT_RENDERER,
                             "Trying to free a NULL renderer");
        return;
    }

    if (rendr->bound_index_array_buffer) {
        sa_Free_m(rendr->bound_index_array_buffer);
        rendr->bound_index_array_buffer = NULL;
    }

    if (rendr->bound_uniform_data) {
        for (sa_u32 i = 0; i < rendr->bound_uniform_data_capacity; ++i) {
            if (rendr->bound_uniform_data[i].value) {
                sa_Free_m(rendr->bound_uniform_data[i].value);
                rendr->bound_uniform_data[i].value = NULL;
            }
        }
        sa_Free_m(rendr->bound_uniform_data);
    }

    s_Renderer_Free_Batch_Array(rendr);
}

SA_INTERNAL void s_Renderer_Reset_Bound(struct sc_renderer* rendr) {
    rendr->bound_texture_id = 0;         // TODO change to macro
    rendr->bound_index_array_length = 0; // TODO change to a macro
}

SA_INTERNAL void s_Renderer_Reset_Batch(struct sc_renderer* rendr) {
    sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                         sa_LOG_CONTEXT_RENDERER,
                         "Reseting batches");
    for (sa_u8 i = 0; i < rendr->batch_array_capacity; ++i) {
        rendr->dynamic_batch_array[i].texture = 0;
        rendr->dynamic_batch_array[i].index_array_length = 0;
        rendr->dynamic_batch_array[i].vertex_array_length = 0;
        for (sa_u32 j = 0; j < rendr->bound_uniform_data_capacity; ++j) {
            sa_Free_m(rendr->dynamic_batch_array[i].bound_uniform_data[j].value);
            rendr->dynamic_batch_array[i].bound_uniform_data[j].value = NULL;
            rendr->dynamic_batch_array[i].bound_uniform_data[j].type = 0;
        }
    }
    rendr->batch_in_use = 0;
}

SA_INTERNAL sa_bool s_Renderer_Validate_Before_Push(const struct sc_renderer* rendr,
                                                    const sa_vec3Array* pos_array,
                                                    const sa_uvArray* uv_array,
                                                    const sa_colorArray* color_array) {
    if (!pos_array) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER,
                             "Vertex array being pushed has length ZERO or is NULL");
        return sa_FALSE;
    }
    if (!pos_array->data || !pos_array->length) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER,
                             "Vertex array being pushed has length ZERO or is NULL");
        return sa_FALSE;
    }
    if (pos_array->length != uv_array->length || pos_array->length != color_array->length) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER,
                             "Renderer doesn't have a 1-1-1 vertex-uv-color ratio");
        return sa_FALSE;
    }
    if (!rendr->bound_index_array_buffer || !rendr->bound_index_array_length) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_RENDERER, "NULL index array bound");
        return sa_FALSE;
    }

    sa_Log_Debug_Condition_Print_m(uv_array, sa_LOG_DEBUG_TYPE_RENDERER,
                                   sa_LOG_CONTEXT_OPENGL, "NULL uv array param");

    sa_Log_Debug_Condition_Print_m(color_array, sa_LOG_DEBUG_TYPE_RENDERER,
                                   sa_LOG_CONTEXT_OPENGL, "NULL color array param");

    return sa_TRUE;
}

SA_INTERNAL void s_Renderer_Initialize_Config(struct sc_renderer* rendr, const char* cfg_path) {
    sc_configState* cfg_state = sc_Config_Load(
        cfg_path);

    // Default values
    rendr->bound_index_array_capacity = SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;
    rendr->batch_index_capacity = SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY;
    rendr->batch_array_capacity = SC_RENDERER_DEFAULT_BATCH_CAPACITY;
    rendr->batch_vertex_capacity = SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY;
    if (!sc_Config_Load_Table(cfg_state, "core_renderer")) {
        return;
    }
    rendr->bound_index_array_capacity =
        sc_Config_Get_Int32(cfg_state, "bound_index_array_capacity");

    rendr->batch_index_capacity =
        sc_Config_Get_Int32(cfg_state, "batch_index_capacity");

    rendr->batch_array_capacity =
        sc_Config_Get_Int8(cfg_state, "batch_array_capacity");

    rendr->batch_vertex_capacity =
        sc_Config_Get_Int32(cfg_state, "batch_vertex_capacity");
}

SA_INTERNAL void s_Renderer_Init_GL(struct sc_renderer* rendr) {
#ifndef SACI_RENDERING_DISABLED
    { // Shader init
        SA_INTERNAL const char* s_vert_shader =
#  ifndef SC_RENDERER_DEFAULT_VERT_SHADER
#    define SC_RENDERER_DEFAULT_VERT_SHADER
            // TODO Evaluate if there should be multiple shaders, one for 2d, 3d
            // instancing, 3d dynamic mesh etc. Saci-Core needs to be
            // performatic but also "just work", so the user shouldn't need to
            // change it unless they want that '1%' boost
            "#version 330 core\n"

            "layout (location = 0) in vec3 a_pos;\n"
            "layout (location = 1) in vec4 a_color;\n"
            "layout (location = 2) in vec2 a_texcoord;\n"
            "layout (location = 3) in mat4 a_model_matrix;\n"

            "layout(std140) uniform Uniforms {\n"
            "    mat4 u_model_matrix;\n"
            "    mat4 u_view_matrix;\n"
            "    mat4 u_projection_matrix;\n"
            "    int u_flags;\n"
            "    vec4 u_lighting;\n"
            "};\n"

            "out vec4 v_color;\n"
            "out vec2 v_texcoord;\n"

            "void main()\n"
            "{\n"
            "   vec4 world_position = u_model_matrix * a_model_matrix * vec4(a_pos, 1.0);\n"
            "   if (u_flags & 0b01 == 1) {\n"
            "       gl_Position = u_projection_matrix * u_view_matrix * world_position;\n"
            "   } else {\n"
            "       gl_Position = world_position;\n"
            "   }\n"
            "   v_color = a_color;\n"
            "   v_texcoord = a_texcoord;\n"
            "}\n\0";
#  endif // SC_RENDERER_DEFAULT_VERT_SHADER

        SA_INTERNAL const char* s_frag_shader =
#  ifndef SC_RENDERER_DEFAULT_FRAG_SHADER
#    define SC_RENDERER_DEFAULT_FRAG_SHADER
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
#  endif // SC_RENDERER_DEFAULT_FRAG_SHADER
        sa_shaderId v_shader = sc_Shader_Compile_Shader_Vert(s_vert_shader);
        sa_shaderId f_shader = sc_Shader_Compile_Shader_Frag(s_frag_shader);
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
        glBufferData(GL_UNIFORM_BUFFER, SC_RENDERER_DEFAULT_UBO_SIZE, NULL, GL_DYNAMIC_DRAW);
        sc_GL_Create_Vertex_Array(1, &rendr->vao);
        sa_Log_Assert_Message_m(rendr->vao, "VAO could not be initialized");
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

SA_INTERNAL void s_Renderer_Init_Batch(struct sc_renderer* rendr) {
    rendr->batch_in_use = 0;
    rendr->dynamic_batch_array = sa_Calloc_m(rendr->batch_array_capacity, sizeof(struct sc_dynamicBatch));
    sa_Log_Assert_Message_m(rendr->dynamic_batch_array, "Batch array could not be initialized");
    rendr->bound_uniform_data = sa_Calloc_m(rendr->bound_uniform_data_capacity, sizeof(struct sc_uniformData));

    for (sa_u32 i = 0; i < rendr->batch_array_capacity; ++i) {
        rendr->dynamic_batch_array[i].vertex_array =
            sa_Calloc_m(rendr->batch_vertex_capacity, sizeof(struct sc_vertex));
        sa_Log_Assert_Message_m(rendr->dynamic_batch_array[i].vertex_array,
                                "Batch vertex array could not be initialized");

        rendr->dynamic_batch_array[i].bound_uniform_data = sa_Calloc_m(rendr->bound_uniform_data_capacity, sizeof(struct sc_uniformData));
        rendr->dynamic_batch_array[i].index_array = sa_Calloc_m(rendr->batch_index_capacity,
                                                                sizeof(sa_u32));
        sa_Log_Assert_Message_m(rendr->dynamic_batch_array[i].index_array,
                                "Batch index array could not be initialized");
    }
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

SA_INTERNAL sa_u32 s_Renderer_Vertex_Array_Append(sc_vertexArray* dest_out,
                                                  const sa_vec3Array* src_pos,
                                                  const sa_uvArray* src_uv,
                                                  const sa_colorArray* src_color,
                                                  sa_u32 src_count) {
    sa_Log_Assert_Message_m(dest_out, "Invalid pointer for vertex array");
    sa_u32 remaining_vertices = sa_Scast_To_m(sa_u32)(src_count - dest_out->length);
    sa_u32 vertices_to_copy = sa_Min_m(sa_Scast_To_m(sa_u32)(dest_out->capacity),
                                       remaining_vertices);
    for (sa_u32 i = 0; i < vertices_to_copy; ++i) {
        sa_u32 idx = sa_Scast_To_m(sa_u32)(dest_out->length + i);
        sa_vec3 pos = sa_Vec3_Array_Get(src_pos, idx);
        sa_uv uv = (!src_uv->data ? (sa_uv){0, 0} : sa_Uv_Array_Get(src_uv, idx));
        sa_color color = (!src_color ? (sa_color){0, 0, 0, 1} : sa_Color_Array_Get(src_color, idx));
        sc_Vertex_Array_Set(dest_out, i, (sc_vertex){pos, color, uv});
    }
    return vertices_to_copy;
}

SA_INTERNAL sa_bool s_Renderer_Batch_Can_Push(const sc_renderer* rendr, const struct sc_dynamicBatch* batch) {
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
    return sa_TRUE;
}

SA_INTERNAL void s_Renderer_Draw_Dynamic_Batch(const sc_renderer* rendr) {
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
                        batch->vertex_array);

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
}

/* === OpenGL === */

SA_API void sc_GL_Uniform_Set_Value(const sa_u32 location, sa_dataType type, const void* value) {
    switch (type) {
    case SA_TYPE_U8:
    case SA_TYPE_U16:
    case SA_TYPE_U32:
    case SA_TYPE_U64:
    case SA_TYPE_SHADERID:
    case SA_TYPE_TEXTUREID:
    case SA_TYPE_BUFFERID:
        glUniform1ui((GLint)location, *(const GLint*)value);
        break;

    case SA_TYPE_BOOL:
    case SA_TYPE_S8:
    case SA_TYPE_S16:
    case SA_TYPE_S32:
    case SA_TYPE_S64:
        glUniform1i((GLint)location, *(const GLint*)value);
        break;

    case SA_TYPE_UV:
    case SA_TYPE_VEC2:
        glUniform2f((GLint)location, ((const GLfloat*)value)[0], ((const GLfloat*)value)[1]);
        break;

    case SA_TYPE_VEC3:
        glUniform3f((GLint)location,
                    ((const float*)value)[0],
                    ((const float*)value)[1],
                    ((const float*)value)[2]);
        break;

    case SA_TYPE_VEC4:
    case SA_TYPE_COLOR:
        glUniform4f((GLint)location,
                    ((const float*)value)[0],
                    ((const float*)value)[1],
                    ((const float*)value)[2],
                    ((const float*)value)[3]);
        break;

    case SA_TYPE_MAT2:
        glUniformMatrix2fv((GLint)location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT3:
        glUniformMatrix3fv((GLint)location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT4:
        glUniformMatrix4fv((GLint)location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT2X3:
        glUniformMatrix2x3fv((GLint)location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT2X4:
        glUniformMatrix2x4fv((GLint)location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT3X2:
        glUniformMatrix3x2fv((GLint)location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT3X4:
        glUniformMatrix3x4fv((GLint)location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT4X2:
        glUniformMatrix4x2fv((GLint)location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    case SA_TYPE_MAT4X3:
        glUniformMatrix4x3fv((GLint)location, 1, GL_FALSE, (const GLfloat*)value);
        break;

    default:
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_OPENGL,
                              "Invalid type %d for uniform", type);
        break;
    }
}

SA_API void sc_GL_Resize_Vertex_Buffer(sa_u32 vao_id, sa_u32 vbo_id, sa_u64 new_size) {
    glBindVertexArray(vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sa_Scast_To_m(sa_s64)(new_size), NULL, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

SA_API sa_u32 sc_GL_Create_Index_Buffer_Dynamic(sa_u32* indices, sa_u64 indice_amount) {
    sa_u32 ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sa_Scast_To_m(sa_s64)(indice_amount * sizeof(sa_u32)), &indices[0],
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return ibo;
}

SA_API sa_u32 sc_GL_Create_Index_Buffer_Static(sa_u32* indices, sa_u64 indice_amount) {
    sa_u32 ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sa_Scast_To_m(sa_s64)(indice_amount * sizeof(sa_u32)), &indices[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return ibo;
}

SA_API void sc_GL_Create_Vertex_Array(sa_u64 size, sa_u32* arrays) {
    glGenVertexArrays(sa_Scast_To_m(int)(size), arrays);
}

SA_API void sc_GL_Bind_Vertex_Array(sa_u32 array) {
    glBindVertexArray(array);
}

SA_API void sc_GL_Bind_Vertex_Buffer(sa_u32 vbo) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

SA_API sa_u32 sc_GL_Create_Vertex_Buffer(sa_u64 size, const void* data, sa_u32 usage) {
    sa_u32 vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sa_Scast_To_m(long int)(size), data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vbo;
}

SA_API void sc_GL_Set_Vertex_Attrib_Pointer(sa_u32 index, int size, sa_u32 type, sa_bool normalized, sa_u64 stride, void* ptr) {
    glVertexAttribPointer(index, size, type, normalized, sa_Scast_To_m(int)(stride), ptr);
}

void sc_GL_Enable_Vertex_Attrib_Array(sa_u32 id) {
    glEnableVertexAttribArray(id);
}

/* === GL Implementation === */

SA_INTERNAL sa_u32 s_Shader_Compile(const char* shader_source, sa_u32 shader_type);

sa_u32 sc_Shader_Compile_Shader_Vert(const char* source) {
    return s_Shader_Compile(source, GL_VERTEX_SHADER);
}

sa_u32 sc_Shader_Compile_Shader_Frag(const char* source) {
    return s_Shader_Compile(source, GL_FRAGMENT_SHADER);
}

sa_u32 sc_Shader_Compile_Shader_Geom(const char* source) {
    return s_Shader_Compile(source, GL_GEOMETRY_SHADER);
}

sa_u32 sc_Shader_Create_Shader_Program(sa_u32 vshader, sa_u32 fshader) {
    sa_u32 program_id = glCreateProgram();
    glAttachShader(program_id, vshader);
    glAttachShader(program_id, fshader);
    glLinkProgram(program_id);

    sa_s32 success = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char gl_err_message[1024];
        int size_returned = 0;
        glGetProgramInfoLog(program_id, 2048, &size_returned, gl_err_message);
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, "Shader program couldn't be loaded: %s", gl_err_message);
        return 0;
    }
    glDetachShader(program_id, vshader);
    glDetachShader(program_id, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    sa_Log_InfoF_Print_m(sa_LOG_CONTEXT_OPENGL,
                         "Shader program %d loaded successfully", program_id);
    return program_id;
}

sa_u32 sc_Shader_Create_Shader_Program_Geom(sa_u32 vshader, sa_u32 fshader, sa_u32 gshader) {
    sa_u32 program_id = glCreateProgram();
    glAttachShader(program_id, vshader);
    glAttachShader(program_id, fshader);
    glAttachShader(program_id, gshader);
    glLinkProgram(program_id);

    sa_s32 success = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char gl_err_message[1024];
        int size_returned = 0;
        glGetProgramInfoLog(program_id, 2048, &size_returned, gl_err_message);
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, "Shader program couldn't be loaded: %s", gl_err_message);
        return 0;
    }
    glDetachShader(program_id, vshader);
    glDetachShader(program_id, fshader);
    glDetachShader(program_id, gshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    glDeleteShader(gshader);
    sa_Log_InfoF_Print_m(sa_LOG_CONTEXT_OPENGL,
                         "Shader program %d loaded successfully", program_id);

    return program_id;
}

SA_API sa_u32 sc_GL_Uniform_Location(sa_shaderId program_id, const char* const name) {
    return glGetUniformLocation(program_id, name);
}

/* === GL Helper ===  */

SA_INTERNAL sa_u32 s_Shader_Compile(const char* shader_source, sa_u32 shader_type) {
    sa_u32 shader_id = glCreateShader(shader_type);

    glShaderSource(shader_id, 1, &shader_source, NULL);
    glCompileShader(shader_id);

    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char err_message[2048];
        int size_returned = 0;
        glGetShaderInfoLog(shader_id, 2048, &size_returned, &err_message[0]);

        glDeleteShader(shader_id);
        { // Logging
            // TODO redo
            char* log_message = "";
            if (shader_type == GL_VERTEX_SHADER) {
                log_message = "Vertex shader couldn't be loaded";
            }
            if (shader_type == GL_FRAGMENT_SHADER) {
                log_message = "Fragment shader couldn't be loaded";
            }
            if (shader_type == GL_GEOMETRY_SHADER) {
                log_message = "Geometry shader couldn't be loaded";
            }
            sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, log_message);
        }
        return 0;
    }
    {
        // TODO redo
        char* log_message = "";
        if (shader_type == GL_VERTEX_SHADER) {
            log_message = "Vertex shader loaded succesfully";
        }
        if (shader_type == GL_FRAGMENT_SHADER) {
            log_message = "Fragment shader loaded succesfully";
        }
        if (shader_type == GL_GEOMETRY_SHADER) {
            log_message = "Geometry shader loaded succesfully";
        }
        sa_Log_Info_Print_m(sa_LOG_CONTEXT_OPENGL, log_message);
    }

    return shader_id;
}
