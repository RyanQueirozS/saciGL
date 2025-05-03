// TODO uniform_struct_block needs to have an option to be freed, but also need
// to evaludate if it will free the user's uniform_struct_block. Perhaps a copy
// would be nice
#include <glad/glad.h>

#include "saci-core/sc-gl.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-types.h"
#include "saci-core/sc-model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* === Event === */

SA_API void sc_Event_Poll(void) {
    glfwPollEvents();
}

SA_API void sc_Event_Wait(void) {
    glfwWaitEvents();
}

SA_API void sc_Event_Wait_For_Timeout(double timeout) {
    glfwWaitEventsTimeout(timeout);
}

SA_API void sc_Event_Post_Empty(void) {
    glfwPostEmptyEvent();
}

SA_API void sc_Event_Set_Mouse_Pos_Handler(sc_window_t* window, sc_event_mousePosHandler_t mouse_pos_handler) {
    glfwSetCursorPosCallback(window, mouse_pos_handler);
}

SA_API sa_bool sc_Event_Is_Key_Pressed(sc_window_t* window, int keycode) {
    return glfwGetKey(window, sa_Scast_To_m(int)(keycode)) == GLFW_PRESS;
}

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
#  define SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY sa_Scast_To_m(sa_u64)(SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY * 3)
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

#ifndef SC_RENDERER_DEFAULT_CALL_RATIO
#  define SC_RENDERER_DEFAULT_CALL_RATIO 20
#endif // SC_RENDERER_DEFAULT_CALL_RATIO

#ifndef SC_RENDERER_DEFAULT_CALL_INDEX_CAPACITY
#  define SC_RENDERER_DEFAULT_CALL_INDEX_CAPACITY (SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY / SC_RENDERER_DEFAULT_CALL_RATIO)
#endif // SC_RENDERER_DEFAULT_CALL_INDEX_CAPACITY

#ifndef SC_RENDERER_DEFAULT_CALL_VERTEX_CAPACITY
#  define SC_RENDERER_DEFAULT_CALL_VERTEX_CAPACITY (SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY / SC_RENDERER_DEFAULT_CALL_RATIO)
#endif // SC_RENDERER_DEFAULT_CALL_VERTEX_CAPACITY

#ifndef SC_RENDERER_DEFAULT_CALL_CAPACITY
#  define SC_RENDERER_DEFAULT_CALL_CAPACITY SC_RENDERER_DEFAULT_CALL_RATIO
#endif // SC_RENDERER_DEFAULT_CALL_CAPACITY

#ifndef SC_RENDERER_DEFAULT_VERT_SHADER
#  define SC_RENDERER_DEFAULT_VERT_SHADER
SA_INTERNAL const char* sc_vert_shader =
    "#version 330 core\n"

    "layout (location = 0) in vec3 a_pos;\n"
    "layout (location = 1) in vec4 a_color;\n"
    "layout (location = 2) in vec2 a_texcoord;\n"

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
    "   vec4 world_position = u_model_matrix * vec4(a_pos, 1.0);\n"
    "   if (u_flags == 1) {\n"
    "       gl_Position = u_projection_matrix * u_view_matrix * world_position;\n"
    "   } else {\n"
    "       gl_Position = world_position;\n"
    "   }\n"
    "   v_color = a_color;\n"
    "   v_texcoord = a_texcoord;\n"
    "}\n\0";
#endif // SC_RENDERER_DEFAULT_VERT_SHADER

#ifndef SC_RENDERER_DEFAULT_FRAG_SHADER
#  define SC_RENDERER_DEFAULT_FRAG_SHADER
SA_INTERNAL const char* sc_frag_shader =
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
#endif // SC_RENDERER_DEFAULT_FRAG_SHADER

#ifndef SC_RENDERER_STRUCT
#  define SC_RENDERER_STRUCT

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
    sa_u8 batch_length;

    sa_u32 call_index_capacity;
    sa_u32 call_vertex_capacity;
    sa_u8 call_array_capacity;
    sa_u8 call_in_use; // 0 indexed

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

    struct sc_renderCall {
        sa_u64 uniform_struct_block_size;
        sa_textureId texture;
        sa_u32 index_array_length;
        sa_u32 vertex_array_length;
        sa_u32* index_array;
        struct sc_vertex* vertex_array;
        sa_u8* uniform_struct_block;
    }* call_array;

    sa_u32* bound_index_array_buffer;
};

#endif // SC_RENDERER_STRUCT

/* --- Renderer Helper --- */

// Prefere to use this instead of directly changing the vertices to 0.
#define sc_Renderer_Free_OpenGL_m(rendr)          \
    do {                                          \
        glDeleteBuffers(1, &(rendr)->ibo);        \
        glDeleteBuffers(1, &(rendr)->vbo);        \
        glDeleteBuffers(1, &(rendr)->vbo);        \
        glDeleteVertexArrays(1, &(rendr)->vao);   \
        glDeleteProgram((rendr)->shader_program); \
    } while (0)

SA_INTERNAL void sc_Renderer_Free_Batch_Array_s(struct sc_renderer* rendr) {
    if (rendr->batch_array) {

        for (sa_u8 i = 0; i < rendr->batch_array_capacity; ++i) {
            if (rendr->batch_array[i].index_array) {
                sa_Free_m(rendr->batch_array[i].index_array);
                rendr->batch_array[i].index_array = NULL;
            }
            if (rendr->batch_array[i].vertex_array) {
                sa_Free_m(rendr->batch_array[i].vertex_array);
                rendr->batch_array[i].vertex_array = NULL;
            }
            if (rendr->batch_array[i].uniform_struct_block) {
                sa_Free_m(rendr->batch_array[i].uniform_struct_block);
                rendr->batch_array[i].uniform_struct_block = NULL;
            }
        }
        sa_Free_m(rendr->batch_array);
        rendr->batch_array = NULL;
    }
}

SA_INTERNAL void sc_Renderer_Free_Call_Array_s(struct sc_renderer* rendr) {
    if (rendr->call_array) {
        for (sa_u8 i = 0; i < rendr->call_array_capacity; ++i) {
            if (rendr->call_array[i].index_array) {
                sa_Free_m(rendr->call_array[i].index_array);
                rendr->call_array[i].index_array = NULL;
            }
            if (rendr->call_array[i].vertex_array) {
                sa_Free_m(rendr->call_array[i].vertex_array);
                rendr->call_array[i].vertex_array = NULL;
            }
            if (rendr->call_array[i].uniform_struct_block) {
                sa_Free_m(rendr->call_array[i].uniform_struct_block);
                rendr->call_array[i].uniform_struct_block = NULL;
            }
        }
        sa_Free_m(rendr->call_array);
        rendr->call_array = NULL;
    }
}

SA_INTERNAL void sc_Renderer_Free_Memory_s(struct sc_renderer* rendr) {
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

    if (rendr->uniform_struct_block) {
        sa_Free_m(rendr->uniform_struct_block);
        rendr->uniform_struct_block = NULL;
    }

    sc_Renderer_Free_Batch_Array_s(rendr);
    sc_Renderer_Free_Call_Array_s(rendr);
}

SA_INTERNAL void sc_Renderer_Reset_Bound_s(struct sc_renderer* rendr) {
    rendr->bound_texture_id = 0;         // TODO change to macro
    rendr->bound_index_array_length = 0; // TODO change to a macro
}

SA_INTERNAL void sc_Renderer_Reset_Batch_s(struct sc_renderer* rendr) {
    for (sa_u8 i = 0; i < rendr->batch_array_capacity; ++i) {
        rendr->batch_array[i].uniform_struct_block_size = 0;
        rendr->batch_array[i].texture = 0;
        rendr->batch_array[i].index_array_length = 0;
        rendr->batch_array[i].vertex_array_length = 0;
        if (rendr->batch_array[i].uniform_struct_block) {
            sa_Free_m(rendr->batch_array[i].uniform_struct_block);
            rendr->batch_array[i].uniform_struct_block = NULL;
        }
        rendr->batch_array[i].uniform_struct_block_size = 0;
    }
    rendr->batch_length = 0;
}

SA_INTERNAL void sc_Renderer_Reset_Call_s(struct sc_renderer* rendr) {
    for (sa_u8 i = 0; i < rendr->call_array_capacity; ++i) {
        rendr->call_array[i].uniform_struct_block_size = 0;
        rendr->call_array[i].texture = 0;
        rendr->call_array[i].index_array_length = 0;
        rendr->call_array[i].vertex_array_length = 0;
        if (rendr->call_array[i].uniform_struct_block) {
            sa_Free_m(rendr->call_array[i].uniform_struct_block);
            rendr->call_array[i].uniform_struct_block = NULL;
        }
        rendr->call_array[i].uniform_struct_block_size = 0;
    }
    rendr->call_in_use = 0;
}

SA_INTERNAL sa_bool sc_Renderer_Validate_Before_Push(const struct sc_renderer* rendr,
                                                     const sa_vec3* pos_array,
                                                     const sa_uv* uv_array,
                                                     const sa_color* color_array,
                                                     const sa_u32 vertex_amount) {
    if (!pos_array || vertex_amount < 1) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER,
                             "Vertex array being pushed has length ZERO or is NULL");
        return sa_FALSE;
    }
    if (!rendr->bound_index_array_buffer || !rendr->bound_index_array_length) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_RENDERER, "NULL index array bound");
        return sa_FALSE;
    }
    if (!rendr->uniform_struct_block || !rendr->bound_uniform_struct_size) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER, "NULL uniform struct array bound");
        return sa_FALSE;
    }

    if (!uv_array) {
        sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER, sa_LOG_CONTEXT_OPENGL, "Null uv array param");
    }

    if (!color_array) {
        sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER, sa_LOG_CONTEXT_OPENGL, "Null color array param");
    }
    return sa_TRUE;
}

SA_INTERNAL void sc_Renderer_Init_s(struct sc_renderer* rendr) {
    { // Shader init
        sa_shaderId v_shader = sc_Shader_Compile_Shader_Vert(sc_vert_shader);
        sa_shaderId f_shader = sc_Shader_Compile_Shader_Frag(sc_frag_shader);
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
        glGenBuffers(1, &rendr->ubo);
        sa_Log_Assert_Message_m(rendr->ubo, "UBO could not be initialized");
        glBindBuffer(GL_UNIFORM_BUFFER, rendr->ubo);
        glBufferData(GL_UNIFORM_BUFFER, SC_RENDERER_DEFAULT_UBO_SIZE, NULL, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, SC_RENDERER_DEFAULT_UBO_BINDING_POINT, rendr->ubo);
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
    }
}

SA_INTERNAL void sc_Renderer_Init_Batch_s(struct sc_renderer* rendr) {
    rendr->batch_length = 0;
    rendr->batch_array = sa_Calloc_m(rendr->batch_array_capacity, sizeof(struct sc_renderBatch));
    sa_Log_Assert_Message_m(rendr->batch_array, "Batch array could not be initialized");

    for (sa_u32 i = 0; i < rendr->batch_array_capacity; ++i) {
        rendr->batch_array[i].vertex_array =
            sa_Calloc_m(rendr->batch_vertex_capacity, sizeof(struct sc_vertex));
        sa_Log_Assert_Message_m(rendr->batch_array[i].vertex_array,
                                "Batch vertex array could not be initialized");
        rendr->batch_array[i].index_array =
            sa_Calloc_m(rendr->batch_index_capacity, sizeof(sa_u32));
        sa_Log_Assert_Message_m(rendr->batch_array[i].index_array,
                                "Batch index array could not be initialized");
    }
}

SA_INTERNAL void sc_Renderer_Init_Call_s(struct sc_renderer* rendr) {
    rendr->call_in_use = 0;
    rendr->call_array =
        sa_Calloc_m(rendr->call_array_capacity, sizeof(struct sc_renderCall));
    sa_Log_Assert_Message_m(rendr->call_array, "Call array could not be initialized");
    for (sa_u8 i = 0; i < rendr->call_array_capacity; ++i) {
        rendr->call_array[i].index_array =
            sa_Calloc_m(rendr->call_index_capacity, sizeof(sa_u32));
        sa_Log_Assert_Message_m(rendr->call_array[i].index_array,
                                "Call INDEX array could not be initialized");
        rendr->call_array[i].vertex_array =
            sa_Calloc_m(rendr->call_vertex_capacity, sizeof(struct sc_vertex));
        sa_Log_Assert_Message_m(rendr->call_array[i].vertex_array,
                                "Call VERTEX array could not be initialized");
        // UNIFORM Will be bound by the user and then pushed and dynamically
        // malloced after. TODO: Evaluate if there is a possibility to allocate
        // a buffer for all uniform blocks like an arena
        rendr->call_array[i].uniform_struct_block = NULL;
    }
}

// TODO evaluate if sorting is indeed needed
// Only pass the call_amount that has been changed
SA_INTERNAL void sc_Renderer_Call_Array_Sort_s(struct sc_renderCall* call_array_out, sa_u8 call_amount) {
    sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS, sa_LOG_CONTEXT_RENDERER, "Sorting %d calls", call_amount);
    if (call_amount < 3) { // 0, 1 shouldn't be sorted and 2 won't matter
        return;
    }
    for (sa_u8 i = 0; i < sa_Scast_To_m(sa_u8)(call_amount - 1); ++i) {
        for (sa_u8 j = 0; j < sa_Scast_To_m(sa_u8)(call_amount - i - 1); ++j) {
            struct sc_renderCall* a = &call_array_out[j];
            struct sc_renderCall* b = &call_array_out[j + 1];

            // First compare by texture
            if (a->texture > b->texture ||
                (a->texture == b->texture && a->uniform_struct_block_size > b->uniform_struct_block_size)) {

                // Swap the two calls
                struct sc_renderCall temp = call_array_out[j];
                call_array_out[j] = call_array_out[j + 1];
                call_array_out[j + 1] = temp;
            }
        }
    }
}

SA_INTERNAL sa_bool sc_Renderer_Uniform_Is_Equal_s(sa_u8* u1, sa_u8* u2, sa_u64 size) {
    return memcmp(u1, u2, size) == 0;
}

SA_INTERNAL void sc_Renderer_Batch_Draw_s(struct sc_renderer* rendr) {
    sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS, sa_LOG_CONTEXT_RENDERER, "Flushing %d batches", rendr->batch_length);
    struct previousBatch { // Will get bigger later
        sa_textureId texture;
    } previous_batch = {0};
    for (sa_u8 i = 0; i < rendr->batch_length; ++i) {
        struct sc_renderBatch* batch_in_use = &rendr->batch_array[i];

        glUseProgram(rendr->shader_program);

        glBindVertexArray(rendr->vao);

        glBindBuffer(GL_ARRAY_BUFFER, rendr->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        sa_Scast_To_m(long int)(sizeof(struct sc_vertex) * batch_in_use->vertex_array_length),
                        batch_in_use->vertex_array);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendr->ibo);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                        sa_Scast_To_m(long int)(sizeof(sa_u32) * batch_in_use->index_array_length),
                        batch_in_use->index_array);

        { // Uniforms
            {
                // TODO should be removed
                glEnable(GL_DEPTH_TEST);
            }

            glBindBuffer(GL_UNIFORM_BUFFER, rendr->ubo);
            glBufferSubData(GL_UNIFORM_BUFFER, 0,
                            sa_Scast_To_m(long int)(batch_in_use->uniform_struct_block_size),
                            batch_in_use->uniform_struct_block);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        if (batch_in_use->texture != previous_batch.texture) {
            glUniform1i(SC_U_USE_TEXTURE_LOC, sa_TRUE);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, batch_in_use->texture);
            previous_batch.texture = batch_in_use->texture;
        }

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, rendr->ubo);
        glDrawElements(GL_TRIANGLES, sa_Scast_To_m(int)(batch_in_use->index_array_length), GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

SA_INTERNAL void sc_Renderer_Vertex_Buffer_Append_s(struct sc_vertex* dest_out,
                                                    struct sc_vertex* src,
                                                    sa_u32 dest_vertices_pushed,
                                                    sa_u32 src_vertices_pushed,
                                                    sa_u32 dest_capacity,
                                                    sa_u32 src_count,
                                                    sa_u32* vertices_copied_out) {
    sa_Log_Assert_Message_m(dest_out, "Invalid ptr for vertex array");

    if (dest_vertices_pushed >= dest_capacity) {
        sa_Log_Assert_Message_m(dest_out, "Destination buffer overflow");
        return;
    }
    if (src_vertices_pushed >= src_count) {
        sa_Log_Assert_Message_m(src, "Source buffer overflow");
        return;
    }

    sa_u32 remaining_vertices = src_count - src_vertices_pushed;
    sa_u32 vertex_copy_count = sa_Min_m(dest_capacity - dest_vertices_pushed, remaining_vertices);

    memcpy(&dest_out[dest_vertices_pushed], &src[src_vertices_pushed], sizeof(struct sc_vertex) * vertex_copy_count);
    *vertices_copied_out = vertex_copy_count;
}

SA_INTERNAL void sc_Renderer_Index_Buffer_Append_s(sa_u32* dest_out,
                                                   sa_u32* src,
                                                   sa_u32 dest_indices_pushed,
                                                   sa_u32 src_indices_pushed,
                                                   sa_u32 dest_capacity,
                                                   sa_u32 src_count,
                                                   sa_u32* indices_copied) {
    sa_Log_Assert_Message_m(dest_out, "Invalid pointer for index array");

    if (dest_indices_pushed >= dest_capacity) {
        sa_Log_Assert_Message_m(dest_out, "Destination buffer overflow");
        return;
    }
    if (src_indices_pushed >= src_count) {
        sa_Log_Assert_Message_m(src, "Source buffer overflow");
        return;
    }

    sa_u32 remaining_indices = src_count - src_indices_pushed;
    sa_u32 indices_to_copy = sa_Min_m(dest_capacity - dest_indices_pushed, remaining_indices);
    memcpy(&dest_out[dest_indices_pushed], &src[src_indices_pushed], indices_to_copy * sizeof(sa_u32));
    *indices_copied = indices_to_copy;
}

SA_INTERNAL void sc_Renderer_Batch_Calls_s(struct sc_renderer* rendr) {
    sa_u8 batch_capacity = rendr->batch_array_capacity;
    if (batch_capacity < 1) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER,
                             "Renderer_End called but batch has ZERO capacity");
        return;
    }

    sa_u8 call_amount = rendr->call_in_use;
    if (call_amount == 0) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_LOW,
                             sa_LOG_CONTEXT_RENDERER,
                             "Renderer_End called but renderer has no calls");
        return;
    }

    struct sc_renderCall* call_array = rendr->call_array;
    struct sc_renderBatch* batch_array = rendr->batch_array;
    sa_Log_Assert_Message_m(call_array, "Could not point to call array");
    sa_Log_Assert_Message_m(batch_array, "Could not point to batch array");

    sa_u8 max_batch_reached = 0;
    sa_u32 call_vertices_pushed = 0;
    sa_u32 call_indices_pushed = 0;
    for (sa_u8 i = 0; i < call_amount; ++i) {
        struct sc_renderCall* call_now = &call_array[i];
        sa_bool batched = sa_FALSE;

        for (sa_u8 b = 0; b < batch_capacity; ++b) {
            struct sc_renderBatch* batch_now = &batch_array[b];

            // Compare compatibility
            if (batch_now->texture != call_now->texture && batch_now->texture != 0) {
                sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
                                     "Texture in call didn't match the batch");
                continue;
            }
            if (batch_now->uniform_struct_block_size != call_now->uniform_struct_block_size && batch_now->uniform_struct_block_size != 0) {
                sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
                                     "Uniform struct block size  in call didn't match the batch");
                continue;
            }
            if (!sc_Renderer_Uniform_Is_Equal_s(
                    batch_now->uniform_struct_block,
                    call_now->uniform_struct_block,
                    sa_Min_m(call_now->uniform_struct_block_size, batch_now->uniform_struct_block_size))) {
                sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
                                     "Uniforms don't match");
                continue;
            }

            // Insert indices
            sa_u32 indices_copied;
            sc_Renderer_Index_Buffer_Append_s(batch_now->index_array,
                                              call_now->index_array,
                                              batch_now->index_array_length,
                                              call_indices_pushed,
                                              rendr->call_index_capacity,
                                              call_now->index_array_length,
                                              &indices_copied);
            batch_now->index_array_length += indices_copied;

            // Insert vertices
            sa_u32 vertices_copied;
            sc_Renderer_Vertex_Buffer_Append_s(batch_now->vertex_array,
                                               call_now->vertex_array,
                                               batch_now->vertex_array_length,
                                               call_vertices_pushed,
                                               rendr->batch_vertex_capacity,
                                               call_now->vertex_array_length,
                                               &vertices_copied);
            batch_now->vertex_array_length += vertices_copied;

            call_vertices_pushed += vertices_copied;
            call_indices_pushed += indices_copied;

            batch_now->texture = call_now->texture;
            batch_now->uniform_struct_block = sa_Malloc_m(call_now->uniform_struct_block_size);
            memcpy(batch_now->uniform_struct_block, call_now->uniform_struct_block, call_now->uniform_struct_block_size);
            batch_now->uniform_struct_block_size = call_now->uniform_struct_block_size;

            batched = sa_TRUE;
            if (max_batch_reached < b) {
                max_batch_reached = b;
            }

            if (call_vertices_pushed >= call_now->vertex_array_length || call_indices_pushed >= call_now->index_array_length) {
                call_vertices_pushed = 0;
                call_indices_pushed = 0;
                break; // Call succesfully batched
            }
        }

        if (!batched) {
            sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_BATCH, sa_LOG_CONTEXT_RENDERER,
                                 "No suitable batch found. Flushing and retrying.");
            sc_Renderer_Batch_Draw_s(rendr);
            sc_Renderer_Reset_Batch_s(rendr);
            i--; // Retry same call after flush
        }
        rendr->batch_length = max_batch_reached + 1;
    }
}

SA_INTERNAL void sc_Renderer_Push_To_Vertex_Buffer_s(struct sc_vertex* dest_out,
                                                     const sa_vec3* src_pos,
                                                     const sa_uv* src_uv,
                                                     const sa_color* src_color,
                                                     sa_u32 dest_vertices_pushed,
                                                     sa_u32 dest_capacity,
                                                     sa_u32 src_count,
                                                     sa_u32* vertices_copied_out) {
    sa_Log_Assert_Message_m(dest_out, "Invalid pointer for vertex array");
    sa_u32 remaining_vertices = src_count - dest_vertices_pushed;
    sa_u32 vertices_to_copy = sa_Min_m(dest_capacity, remaining_vertices);
    for (sa_u32 i = 0; i < vertices_to_copy; ++i) {
        sa_u32 idx = dest_vertices_pushed + i;
        sa_uv uv = (!src_uv ? (sa_uv){0, 0} : src_uv[idx]);
        sa_color color = (!src_color ? (sa_color){0, 0, 0, 1} : src_color[idx]);
        dest_out[i].pos = src_pos[idx];
        dest_out[i].uv = uv;
        dest_out[i].color = color;
    }
    *vertices_copied_out = vertices_to_copy;
}

/* --- Renderer Header Impl --- */

SA_API sc_renderer* sc_Renderer_New_Default(void) {
    struct sc_renderer* rendr = sa_Calloc_m(1, sizeof(struct sc_renderer));
    sa_Log_Assert_Message_m(rendr, "Renderer could not be created");
    sc_Renderer_Init_s(rendr);
    rendr->batch_array_capacity = SC_RENDERER_DEFAULT_BATCH_CAPACITY;
    rendr->batch_vertex_capacity = SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY;
    rendr->batch_index_capacity = SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY;
    sc_Renderer_Init_Batch_s(rendr);
    rendr->call_array_capacity = SC_RENDERER_DEFAULT_CALL_CAPACITY;
    rendr->call_index_capacity = SC_RENDERER_DEFAULT_CALL_INDEX_CAPACITY;
    rendr->call_vertex_capacity = SC_RENDERER_DEFAULT_CALL_VERTEX_CAPACITY;
    sc_Renderer_Init_Call_s(rendr);

    { // Bound info
        rendr->bound_texture_id = 0;
        rendr->bound_index_array_length = 0;
        rendr->bound_index_array_capacity = SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;
        rendr->bound_index_array_buffer =
            sa_Calloc_m(rendr->bound_index_array_capacity, sizeof(sa_u32));
    }

    return rendr;
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

SA_API void sc_Renderer_Begin(struct sc_renderer* rendr) {
    sc_Renderer_Reset_Bound_s(rendr);
    sc_Renderer_Reset_Batch_s(rendr);
    sc_Renderer_Reset_Call_s(rendr);
}

SA_API void sc_Renderer_Bind_Texture(struct sc_renderer* rendr, sa_textureId tex_id) {
    rendr->bound_texture_id = tex_id;
}

SA_API void sc_Renderer_Set_Uniform_Struct(struct sc_renderer* rendr, sa_u64 size) {
    rendr->bound_uniform_struct_size = size;
    rendr->uniform_struct_block = sa_Malloc_m(rendr->bound_uniform_struct_size);
}

SA_API void sc_Renderer_Set_Bound_Index_Buffer_Capacity(struct sc_renderer* rendr,
                                                        sa_u32 new_size) {
    rendr->bound_index_array_capacity = new_size;
    sa_Free_m(rendr->bound_index_array_buffer);
    rendr->bound_index_array_buffer = sa_Calloc_m(new_size, sizeof(sa_u32));
}

SA_API void sc_Renderer_Bind_Uniform_Struct(struct sc_renderer* rendr, void* uniform) {
    if (!rendr->uniform_struct_block) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER, "Uniform block not created");
        return;
    }
    if (!uniform) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER, "Invalid uniform structure");
        return;
    }

    memcpy(rendr->uniform_struct_block, uniform, rendr->bound_uniform_struct_size);
}

SA_API void sc_Renderer_Bind_Uniform_Value(struct sc_renderer* rendr, void* value, sa_u64 start_offset, sa_u64 size) {
    if (!rendr->uniform_struct_block) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER, "Uniform block not created");
        return;
    }
    if (start_offset + size > rendr->bound_uniform_struct_size) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER, "Uniform block overflow");
        return;
    }
    memcpy((sa_u8*)rendr->uniform_struct_block + start_offset, value, size);
}

SA_API void sc_Renderer_Bind_Index_Buffer(struct sc_renderer* rendr, const sa_u32* new_indices, const sa_u32 new_indices_count) {
    // Reset the index buffer
    if (!new_indices) {
        sa_Log_Warn_Print_m(sa_LOG_SEVERITY_LOW,
                            sa_LOG_CONTEXT_RENDERER,
                            "Indices are NULL and cannot be bound");
        return;
    }

    if (new_indices_count > rendr->bound_index_array_capacity) {
        sa_Log_WarnF_Print_m(sa_LOG_SEVERITY_MEDIUM,
                             sa_LOG_CONTEXT_RENDERER,
                             "Indice bound of capacity: %d is larger than max capacity: %d",
                             new_indices_count, rendr->bound_index_array_capacity);
        return;
    }

    // Separate to a simpler name
    memcpy(rendr->bound_index_array_buffer, new_indices, new_indices_count * sizeof(sa_u32));
    rendr->bound_index_array_length = new_indices_count;
}

SA_API void sc_Renderer_Push_Vertex(struct sc_renderer* rendr,
                                    const sa_vec3* pos_array,
                                    const sa_uv* uv_array,
                                    const sa_color* color_array,
                                    const sa_u32 vertex_amount) {
    sa_Log_DebugF_Print_m(sa_LOG_DEBUG_TYPE_RENDERER_FUNCTIONS,
                          sa_LOG_CONTEXT_RENDERER,
                          "Attempting to push %d vertices", vertex_amount);
    sc_Renderer_Validate_Before_Push(rendr, pos_array, uv_array, color_array,
                                     vertex_amount);
    sa_u32 total_vertices_pushed = 0;
    sa_u32 total_indices_pushed = 0;
    while (total_vertices_pushed < vertex_amount ||
           total_indices_pushed < rendr->bound_index_array_length) {

        if (rendr->call_in_use >= rendr->call_array_capacity) {
            sc_Renderer_End(rendr);
            sc_Renderer_Reset_Call_s(rendr);
            sc_Renderer_Reset_Batch_s(rendr);
        }

        sa_u32 call_index = rendr->call_in_use;
        struct sc_renderCall* call_in_use = &(rendr->call_array[call_index]);
        sa_Log_Assert_Message_m(call_in_use, "Could not point to call in use");

        { // Vertex
            sa_u32 vertices_copied;
            sc_Renderer_Push_To_Vertex_Buffer_s(call_in_use->vertex_array,
                                                pos_array,
                                                uv_array,
                                                color_array,
                                                total_vertices_pushed,
                                                rendr->call_vertex_capacity,
                                                vertex_amount,
                                                &vertices_copied);
            call_in_use->vertex_array_length += vertices_copied;
            total_vertices_pushed += vertices_copied;
        }

        { // Index
            sa_u32 indices_copied;
            sc_Renderer_Index_Buffer_Append_s(call_in_use->index_array,
                                              rendr->bound_index_array_buffer,
                                              call_in_use->index_array_length,
                                              total_indices_pushed,
                                              rendr->call_index_capacity,
                                              rendr->bound_index_array_length,
                                              &indices_copied);
            call_in_use->index_array_length = indices_copied;
            total_indices_pushed += indices_copied;
        }

        call_in_use->texture = rendr->bound_texture_id;
        call_in_use->uniform_struct_block = sa_Malloc_m(rendr->bound_uniform_struct_size);
        memcpy(call_in_use->uniform_struct_block, rendr->uniform_struct_block, rendr->bound_uniform_struct_size);
        call_in_use->uniform_struct_block_size = rendr->bound_uniform_struct_size;

        ++rendr->call_in_use;
    }
}

SA_API void sc_Renderer_Push_Model(struct sc_renderer* rendr, const sc_modelMesh* model_mesh) {
}

SA_API void sc_Renderer_End(struct sc_renderer* rendr) {
    sc_Renderer_Call_Array_Sort_s(rendr->call_array, rendr->call_in_use);
    sc_Renderer_Batch_Calls_s(rendr);
    sc_Renderer_Batch_Draw_s(rendr);
}

SA_API void sc_Renderer_Free(struct sc_renderer* rendr) {
    sc_Renderer_Begin(rendr);
    sc_Renderer_Free_Memory_s(rendr);
    sc_Renderer_Free_OpenGL_m(rendr);
    sa_Free_m(rendr);
    rendr = NULL;
}

SA_API void sc_Renderer_Free_Opts(struct sc_renderer* rendr, int free_opts) {
    sc_Renderer_Begin(rendr);
    if (free_opts & sc_RENDERER_FREE_OPT_MEMORY) {
        sc_Renderer_Free_Memory_s(rendr);
    }
    if (free_opts & sc_RENDERER_FREE_OPT_OPENGL) {
        sc_Renderer_Free_OpenGL_m(rendr);
    }
    sa_Free_m(rendr);
    rendr = NULL;
}

/* === OpenGL === */

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

static sa_u32 sc_Shader_Compile_s(const char* shader_source, sa_u32 shader_type);

sa_u32 sc_Shader_Compile_Shader_Vert(const char* source) {
    return sc_Shader_Compile_s(source, GL_VERTEX_SHADER);
}

sa_u32 sc_Shader_Compile_Shader_Frag(const char* source) {
    return sc_Shader_Compile_s(source, GL_FRAGMENT_SHADER);
}

sa_u32 sc_Shader_Compile_Shader_Geom(const char* source) {
    return sc_Shader_Compile_s(source, GL_GEOMETRY_SHADER);
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

/* === GL Helper ===  */

static sa_u32 sc_Shader_Compile_s(const char* shader_source, sa_u32 shader_type) {
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
