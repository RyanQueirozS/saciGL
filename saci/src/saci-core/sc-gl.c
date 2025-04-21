// TODO uniform_struct_block needs to have an option to be freed, but also need
// to evaludate if it will free the user's uniform_struct_block. Perhaps a copy
// would be nice
#include <glad/glad.h>

#include "saci-core/sc-gl.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-types.h"

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
#ifndef __SC_TEXTURE_ARRAY_SIZE
#  ifdef _WIN32
// Windows
#    define __SC_TEXTURE_ARRAY_SIZE 16
#  elif defined(__APPLE__) && defined(__MACH__)
// macOS
#    define __SC_TEXTURE_ARRAY_SIZE 32
#  elif defined(__linux__)
// Linux
#    define __SC_TEXTURE_ARRAY_SIZE 16
#  elif defined(__ANDROID__)
// Android
#    define __SC_TEXTURE_ARRAY_SIZE 8
#  else
// Default case
#    define __SC_TEXTURE_ARRAY_SIZE 8
#  endif // _WIN32
#endif   // __SC_TEXTURE_ARRAY_SIZE

#ifndef __SC_RENDERER_UNIFORMS
#  define __SC_RENDERER_UNIFORMS

// TODO test these
// These might change and might not be used later...
#  define __SC_U_MODEL_MATRIX_LOC 0
#  define __SC_U_VIEW_MATRIX_LOC 1
#  define __SC_U_PROJECTION_MATRIX_LOC 2
#  define __SC_U_FLAGS 3
#  define __SC_U_LIGHTING 4
#  define __SC_U_TEXTURE_LOC 5
#  define __SC_U_USE_TEXTURE_LOC 6

#endif // __SC_RENDERER_UNIFORMS

#ifndef __SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY
#  define __SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY (3000)
#endif // __SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY

#ifndef __SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY
#  define __SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY (__SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY * 6 / 4)
#endif // __SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY

#ifndef __SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY
#  define __SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY __SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY
#endif // __SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY

#ifndef __SC_RENDERER_DEFAULT_BATCH_CAPACITY
#  define __SC_RENDERER_DEFAULT_BATCH_CAPACITY (5)
#endif // __SC_RENDERER_DEFAULT_BATCH_CAPACITY

#ifndef __SC_RENDERER_DEFAULT_UBO_SIZE
#  define __SC_RENDERER_DEFAULT_UBO_SIZE (212)
#endif // __SC_RENDERER_DEFAULT_UBO_SIZE

#ifndef __SC_RENDERER_DEFAULT_UBO_BINDING_POINT
#  define __SC_RENDERER_DEFAULT_UBO_BINDING_POINT 0
#endif // __SC_RENDERER_DEFAULT_UBO_BINDING_POINT

#ifndef __SC_RENDERER_DEFAULT_CALL_RATIO
#  define __SC_RENDERER_DEFAULT_CALL_RATIO 10
#endif // __SC_RENDERER_DEFAULT_CALL_RATIO

#ifndef __SC_RENDERER_DEFAULT_CALL_INDEX_CAPACITY
#  define __SC_RENDERER_DEFAULT_CALL_INDEX_CAPACITY (__SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY / __SC_RENDERER_DEFAULT_CALL_RATIO)
#endif // __SC_RENDERER_DEFAULT_CALL_INDEX_CAPACITY

#ifndef __SC_RENDERER_DEFAULT_CALL_VERTEX_CAPACITY
#  define __SC_RENDERER_DEFAULT_CALL_VERTEX_CAPACITY (__SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY / __SC_RENDERER_DEFAULT_CALL_RATIO)
#endif // __SC_RENDERER_DEFAULT_CALL_VERTEX_CAPACITY

#ifndef __SC_RENDERER_DEFAULT_CALL_CAPACITY
#  define __SC_RENDERER_DEFAULT_CALL_CAPACITY __SC_RENDERER_DEFAULT_CALL_RATIO
#endif // __SC_RENDERER_DEFAULT_CALL_CAPACITY

#ifndef __SC_RENDERER_DEFAULT_VERT_SHADER
#  define __SC_RENDERER_DEFAULT_VERT_SHADER
SA_INTERNAL const char* vert_shader =
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
#endif // __SC_RENDERER_DEFAULT_VERT_SHADER

#ifndef __SC_RENDERER_DEFAULT_FRAG_SHADER
#  define __SC_RENDERER_DEFAULT_FRAG_SHADER
SA_INTERNAL const char* frag_shader =
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
#endif // __SC_RENDERER_DEFAULT_FRAG_SHADER

#ifndef SC_RENDERER_STRUCT
#  define SC_RENDERER_STRUCT

struct __sc_vertex {
    sa_vec3 pos;
    sa_color color;
    sa_uv uv;
};

// The fields are structured in a way that enforces minimum memory change over time
struct __sc_renderer {
    sa_textureId bound_texture_id;

    sa_shaderId shader_program;
    sa_bufferId ibo, ubo, vbo, vao;

    sa_u32 bound_index_array_length;
    sa_u32 bound_index_array_capacity;

    sa_u32 batch_index_capacity;
    sa_u32 batch_vertex_capacity;
    sa_u8 batch_array_capacity;
    sa_u8 batch_in_use; // 0 indexed

    sa_u32 call_index_capacity;
    sa_u32 call_vertex_capacity;
    sa_u8 call_array_capacity;
    sa_u8 call_in_use; // 0 indexed

    sa_u64 uniform_struct_size;

    struct __sc_batch {
        sa_u64 uniform_struct_block_size;
        sa_textureId texture;
        sa_u32 index_array_length;
        sa_u32 vertex_array_length;
        sa_u8 uniform_block_size;
        sa_u32* index_array;
        struct __sc_vertex* vertex_array;
        sa_u8* uniform_struct_block;
    }* batch_array;

    sa_u8* uniform_struct_block;

    struct __sc_renderCall {
        sa_u64 uniform_struct_block_size;
        sa_textureId texture;
        sa_u32 index_array_length;
        sa_u32 vertex_array_length;
        sa_u8 uniform_block_size;
        sa_u32* index_array;
        struct __sc_vertex* vertex_array;
        sa_u8* uniform_struct_block;
    }* call_array;

    sa_u32* bound_index_array_buffer;
};

#endif // SC_RENDERER_STRUCT

/* --- Renderer Helper --- */

// Prefere to use this instead of directly changing the vertices to 0.
#define __sc_Renderer_Free_Memory_m(rendr)                \
    do {                                                  \
        if ((rendr)->bound_index_array_buffer) {          \
            sa_Free_m((rendr)->bound_index_array_buffer); \
            (rendr)->bound_index_array_buffer = NULL;     \
        }                                                 \
    } while (0)

#define __sc_Renderer_Free_OpenGL_m(rendr)        \
    do {                                          \
        glDeleteBuffers(1, &(rendr)->ibo);        \
        glDeleteBuffers(1, &(rendr)->vbo);        \
        glDeleteVertexArrays(1, &(rendr)->vao);   \
        glDeleteProgram((rendr)->shader_program); \
    } while (0)

SA_INTERNAL void __sc_Renderer_Reset_Bound(struct __sc_renderer* rendr) {
    rendr->bound_texture_id = 0;         // TODO change to macro
    rendr->bound_index_array_length = 0; // TODO change to a macro
}

SA_INTERNAL void __sc_Renderer_Reset_Batch(struct __sc_renderer* rendr) {
    // + 1 because of 0 index
    for (sa_u8 i = 0; i < rendr->batch_in_use + 1; ++i) {
        rendr->batch_array[i].uniform_struct_block_size = 0;
        rendr->batch_array[i].texture = 0;
        rendr->batch_array[i].index_array_length = 0;
        rendr->batch_array[i].vertex_array_length = 0;
        rendr->batch_array[i].uniform_block_size = 0;
        rendr->batch_array[i].uniform_struct_block = NULL;
    }
    rendr->batch_in_use = 0;
}

SA_INTERNAL void __sc_Renderer_Reset_Call(struct __sc_renderer* rendr) {
    // + 1 because of 0 index
    for (sa_u8 i = 0; i < rendr->call_in_use + 1; ++i) {
        rendr->call_array[i].uniform_struct_block_size = 0;
        rendr->call_array[i].texture = 0;
        rendr->call_array[i].index_array_length = 0;
        rendr->call_array[i].vertex_array_length = 0;
        rendr->call_array[i].uniform_block_size = 0;
        rendr->call_array[i].uniform_struct_block = NULL;
    }
    rendr->call_in_use = 0;
}

SA_INTERNAL void __sc_Renderer_Init(struct __sc_renderer* rendr) {
    { // Shader init
        sa_shaderId v_shader = sc_Shader_Compile_Shader_Vert(vert_shader);
        sa_shaderId f_shader = sc_Shader_Compile_Shader_Frag(frag_shader);
        sa_Log_Assert_Message_m(v_shader && f_shader, "Shaders could not be initialized");
        rendr->shader_program = sc_Shader_Create_Shader_Program(v_shader, f_shader);
        sa_Log_Assert_Message_m(rendr->shader_program, "Shader program could not be initialized");
    }
    { // Opengl buffers
        rendr->vbo = sc_GL_Create_Vertex_Buffer(
            sizeof(struct __sc_vertex) * __SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY,
            NULL, GL_DYNAMIC_DRAW);
        sa_Log_Assert_Message_m(rendr->vbo, "VBO could not be initialized");
        rendr->ibo = sc_GL_Create_Index_Buffer_Dynamic(NULL, __SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY);
        sa_Log_Assert_Message_m(rendr->ibo, "IBO could not be initialized");
        glGenBuffers(1, &rendr->ubo);
        sa_Log_Assert_Message_m(rendr->ubo, "UBO could not be initialized");
        glBindBuffer(GL_UNIFORM_BUFFER, rendr->ubo);
        glBufferData(GL_UNIFORM_BUFFER, __SC_RENDERER_DEFAULT_UBO_SIZE, NULL, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, __SC_RENDERER_DEFAULT_UBO_BINDING_POINT, rendr->ubo);
        sc_GL_Create_Vertex_Array(1, &rendr->vao);
        sa_Log_Assert_Message_m(rendr->vao, "VAO could not be initialized");
    }
    { // VertexAttrib init
        sc_GL_Bind_Vertex_Array(rendr->vao);

        sc_GL_Bind_Vertex_Buffer(rendr->vbo);

        sc_GL_Set_Vertex_Attrib_Pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct __sc_vertex),
                                        sa_Scast_To_m(void*) offsetof(struct __sc_vertex, pos));
        sc_GL_Enable_Vertex_Attrib_Array(0);
        sc_GL_Set_Vertex_Attrib_Pointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct __sc_vertex),
                                        sa_Scast_To_m(void*) offsetof(struct __sc_vertex, color));
        sc_GL_Enable_Vertex_Attrib_Array(1);
        sc_GL_Set_Vertex_Attrib_Pointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct __sc_vertex),
                                        sa_Scast_To_m(void*) offsetof(struct __sc_vertex, uv));
        sc_GL_Enable_Vertex_Attrib_Array(2);
    }
}

SA_INTERNAL void __sc_Renderer_Init_Batch(struct __sc_renderer* rendr) {
    rendr->batch_in_use = 0;
    rendr->batch_array = sa_Malloc_m(sizeof(struct __sc_batch) * rendr->batch_array_capacity);
    sa_Log_Assert_Message_m(rendr->batch_array, "Batch array could not be initialized");

    for (sa_u32 i = 0; i < rendr->batch_array_capacity; ++i) {
        rendr->batch_array[i].index_array_length = 0;
        rendr->batch_array[i].vertex_array_length = 0;
        rendr->batch_array[i].vertex_array =
            sa_Malloc_m(sizeof(struct __sc_vertex) * rendr->batch_vertex_capacity);
        sa_Log_Assert_Message_m(rendr->batch_array[i].vertex_array,
                                "Batch vertex array could not be initialized");
        rendr->batch_array[i].index_array =
            sa_Malloc_m(sizeof(sa_u32) * rendr->batch_index_capacity);
        sa_Log_Assert_Message_m(rendr->batch_array[i].index_array,
                                "Batch index array could not be initialized");
    }
}

SA_INTERNAL void __sc_Renderer_Init_Call(struct __sc_renderer* rendr) {
    rendr->call_in_use = 0;
    rendr->call_array =
        sa_Malloc_m(sizeof(struct __sc_renderCall) * rendr->call_array_capacity);
    sa_Log_Assert_Message_m(rendr->call_array, "Call array could not be initialized");
    for (sa_u8 i = 0; i < rendr->call_array_capacity; ++i) {
        rendr->call_array[i].index_array =
            sa_Malloc_m(sizeof(sa_u32) * rendr->call_index_capacity);
        sa_Log_Assert_Message_m(rendr->call_array[i].index_array,
                                "Call INDEX array could not be initialized");
        rendr->call_array[i].vertex_array =
            sa_Malloc_m(sizeof(struct __sc_vertex) * rendr->call_vertex_capacity);
        sa_Log_Assert_Message_m(rendr->call_array[i].vertex_array,
                                "Call VERTEX array could not be initialized");
        rendr->call_array[i].vertex_array_length = 0;
        rendr->call_array[i].index_array_length = 0;
        rendr->call_array[i].texture = 0;
        rendr->call_array[i].uniform_block_size = 0;
        // UNIFORM Will be bound by the user and then pushed and dynamically
        // malloced after. TODO: Evaluate if there is a possibility to allocate
        // a buffer for all uniform blocks like an arena
        rendr->call_array[i].uniform_struct_block = NULL;
    }
}

// TODO evaluate if sorting is indeed needed
// Only pass the call_amount that has been changed
SA_INTERNAL void __sc_Renderer_Call_Array_Sort(struct __sc_renderCall* call_array, sa_u8 call_amount) {
    if (call_amount < 3) { // 0, 1 shouldn't be sorted and 2 won't matter
        return;
    }
    for (sa_u8 i = 0; i < call_amount - 1; ++i) {
        for (sa_u8 j = 0; j < call_amount - i - 1; ++j) {
            struct __sc_renderCall* a = &call_array[j];
            struct __sc_renderCall* b = &call_array[j + 1];

            // First compare by texture
            if (a->texture > b->texture ||
                (a->texture == b->texture && a->uniform_struct_block_size > b->uniform_struct_block_size)) {

                // Swap the two calls
                struct __sc_renderCall temp = call_array[j];
                call_array[j] = call_array[j + 1];
                call_array[j + 1] = temp;
            }
        }
    }
}

SA_INTERNAL sa_bool __sc_Renderer_Uniform_Is_Equal(sa_u8* u1, sa_u8* u2, sa_u64 size) {
    return memcmp(u1, u2, size) == 0;
}

SA_INTERNAL void __sc_Renderer_Batch_Flush(struct __sc_renderer* rendr) {
    for (sa_u8 i = 0; i < rendr->batch_in_use + 1; ++i) { // +1 because of 0 index
        struct __sc_batch* batch_in_use = &rendr->batch_array[i];

        glUseProgram(rendr->shader_program);

        glBindVertexArray(rendr->vao);

        glBindBuffer(GL_ARRAY_BUFFER, rendr->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        sa_Scast_To_m(long int)(sizeof(struct __sc_vertex) * batch_in_use->vertex_array_length),
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
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sa_Scast_To_m(long int)(rendr->uniform_struct_size), rendr->uniform_struct_block);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        if (rendr->bound_texture_id != 0) {
            glUniform1i(__SC_U_USE_TEXTURE_LOC, sa_TRUE);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rendr->bound_texture_id);
        }

        glDrawElements(GL_TRIANGLES, sa_Scast_To_m(int)(batch_in_use->index_array_length), GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }
}

SA_INTERNAL void __sc_Renderer_Batch_Calls(struct __sc_renderer* rendr) {
    sa_u64 batch_capacity = rendr->batch_array_capacity;
    if (batch_capacity < 1) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER,
                             "Renderer_End called but batch has ZERO capacity");
        return;
    }

    sa_u64 call_amount = rendr->call_in_use;
    if (call_amount == 0) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_WARN, sa_LOG_SEVERITY_LOW,
                             sa_LOG_CONTEXT_RENDERER,
                             "Renderer_End called but renderer has no calls");
        return;
    }

    struct __sc_renderCall* call_array = rendr->call_array;
    struct __sc_batch* batch_array = rendr->batch_array;

    sa_Log_Assert_Message_m(call_array, "Could not point to call array");
    sa_Log_Assert_Message_m(batch_array, "Could not point to batch array");

    sa_u8 max_batch_reached = 0;
    for (sa_u8 i = 0; i < call_amount; ++i) {
        struct __sc_renderCall* call_now = &call_array[i];
        sa_bool batched = sa_FALSE;

        for (sa_u8 b = 0; b < batch_capacity; ++b) {
            struct __sc_batch* batch_now = &batch_array[b];

            // Compare compatibility
            if (batch_now->texture != call_now->texture)
                continue;
            if (batch_now->uniform_block_size != call_now->uniform_block_size)
                continue;
            if (batch_now->uniform_struct_block_size != call_now->uniform_struct_block_size)
                continue;

            if (!__sc_Renderer_Uniform_Is_Equal(
                    batch_now->uniform_struct_block,
                    call_now->uniform_struct_block,
                    sa_Min_m(call_now->uniform_struct_block_size, batch_now->uniform_struct_block_size))) {

                continue;
            }

            // Check capacity before inserting
            if ((batch_now->vertex_array_length + call_now->vertex_array_length) > rendr->batch_vertex_capacity ||
                (batch_now->index_array_length + call_now->index_array_length) > rendr->batch_index_capacity) {
                continue; // Try next batch
            }

            // Insert indices
            sa_u32 index_offset = batch_now->index_array_length;
            sa_u32 index_copy_size = call_now->index_array_length * sizeof(sa_u32);
            memcpy(&batch_now->index_array[index_offset], call_now->index_array, index_copy_size);
            batch_now->index_array_length += call_now->index_array_length;

            // Insert vertices
            sa_u32 vertex_offset = batch_now->vertex_array_length;
            sa_u32 vertex_copy_size = call_now->vertex_array_length * sizeof(struct __sc_vertex);
            memcpy(&batch_now->vertex_array[vertex_offset], call_now->vertex_array, vertex_copy_size);
            batch_now->vertex_array_length += call_now->vertex_array_length;

            batched = sa_TRUE;
            if (max_batch_reached < b) {
                max_batch_reached = b;
            }
            break; // Call successfully batched
        }

        if (!batched) {
#ifdef SACI_DEBUG_MODE
            sa_Log_Error_Print_m(sa_LOG_TYPE_DEBUG, sa_LOG_SEVERITY_LOW,
                                 sa_LOG_CONTEXT_RENDERER,
                                 "No suitable batch found. Flushing and retrying.");
#endif
            __sc_Renderer_Batch_Flush(rendr);
            i--; // Retry same call after flush
        }
        rendr->batch_in_use = max_batch_reached;
    }
}

/* --- Renderer Header Impl --- */

SA_API sc_renderer* sc_Renderer_New_Default(void) {
    struct __sc_renderer* rendr = sa_Malloc_m(sizeof(struct __sc_renderer));
    sa_Log_Assert_Message_m(rendr, "Renderer could not be created");
    __sc_Renderer_Init(rendr);
    rendr->batch_array_capacity = __SC_RENDERER_DEFAULT_BATCH_CAPACITY;
    rendr->batch_vertex_capacity = __SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY;
    rendr->batch_index_capacity = __SC_RENDERER_DEFAULT_BATCH_INDEX_CAPACITY;
    __sc_Renderer_Init_Batch(rendr);
    rendr->call_array_capacity = __SC_RENDERER_DEFAULT_CALL_CAPACITY;
    rendr->call_index_capacity = __SC_RENDERER_DEFAULT_CALL_INDEX_CAPACITY;
    rendr->call_vertex_capacity = __SC_RENDERER_DEFAULT_CALL_VERTEX_CAPACITY;
    __sc_Renderer_Init_Call(rendr);

    { // Bound info
        rendr->bound_texture_id = 0;
        rendr->bound_index_array_length = 0;
        rendr->bound_index_array_capacity = __SC_RENDERER_DEFAULT_BOUND_INDEX_CAPACITY;
        rendr->bound_index_array_buffer =
            sa_Malloc_m(sizeof(sa_u32) * rendr->bound_index_array_capacity); // 10k vertices
    }

    return rendr;
}

// TODO
#if 0
SA_API struct __sc_renderer* sc_Renderer_New_Default_Ctx(void* mem_ctx, sa_u64 batch_index_capacity, sa_u64 batch_vertex_capacity, sa_u64 bound_capacity) {
    if (!mem_ctx) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                             "Invalid memory context in sc_Renderer_New_Default_Ctx");
        return NULL; // Should crash, but if LOG_ERROR_PRINT_m is rewriten, returns NULL
    }

    // TODO
    struct __sc_renderer* rendr = sa_Malloc_m(sizeof(struct __sc_renderer));
    assert(rendr);
    __sc_Renderer_Init(rendr);

    return rendr;
}
#endif

SA_API void sc_Renderer_Begin(struct __sc_renderer* rendr) {
    __sc_Renderer_Reset_Bound(rendr);
    __sc_Renderer_Reset_Batch(rendr);
    __sc_Renderer_Reset_Call(rendr);
}

SA_API void sc_Renderer_Bind_Texture(struct __sc_renderer* rendr, sa_textureId tex_id) {
    rendr->bound_texture_id = tex_id;
}

SA_API void sc_Renderer_Set_Uniform_Struct(struct __sc_renderer* rendr, sa_u64 size) {
    rendr->uniform_struct_size = size;
    rendr->uniform_struct_block = sa_Malloc_m(rendr->uniform_struct_size);
}

SA_API void sc_Renderer_Bind_Uniform_Struct(struct __sc_renderer* rendr, void* uniform) {
    if (!rendr->uniform_struct_block) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER, "Uniform block not created");
    }
    if (!uniform) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER, "Invalid uniform structure");
    }
    memcpy(rendr->uniform_struct_block, uniform, rendr->uniform_struct_size);
}

SA_API void sc_Renderer_Bind_Uniform_Value(struct __sc_renderer* rendr, void* value, sa_u64 start_offset, sa_u64 size) {
    if (!rendr->uniform_struct_block) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER, "Uniform block not created");
    }
    if (start_offset + size > rendr->uniform_struct_size) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER, "Uniform block overflow");
        return;
    }
    memcpy((sa_u8*)rendr->uniform_struct_block + start_offset, value, size);
}

SA_API void sc_Renderer_Bind_Index_Buffer(struct __sc_renderer* rendr, const sa_u32* new_indices, const sa_u32 new_indices_count) {
    // Reset the index buffer
    if (new_indices_count > rendr->bound_index_array_capacity) {
        return;
    }

    // Separate to a simpler name
    for (sa_u32 i = 0; i < new_indices_count; ++i) {
        rendr->bound_index_array_buffer[i] = new_indices[i];
    }
    rendr->bound_index_array_length = new_indices_count;
}

SA_API void sc_Renderer_Push_Vertex(struct __sc_renderer* rendr, const sa_vec3* pos_array, const sa_uv* uv_array, const sa_color* color_array, const sa_u32 vertex_amount) {
    if (!pos_array || vertex_amount < 1) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER,
                             "Vertex array being pushed has lenght ZERO or is NULL");
        return;
    }
    if (rendr->call_vertex_capacity < vertex_amount) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH,
                             sa_LOG_CONTEXT_RENDERER,
                             "Vertex amount overflows draw call");
        return;
    }
    if (!rendr->bound_index_array_buffer || !rendr->bound_index_array_length) {
        sa_Log_Info_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_CONTEXT_RENDERER, "NULL index array bound");
        return;
    }

    sa_u32 call_index = rendr->call_in_use;
    struct __sc_renderCall* call_in_use = &(rendr->call_array[call_index]);
    sa_Log_Assert_Message_m(call_in_use, "Could not point to call in use");

    { // Vertex operations
        struct __sc_vertex* vertex_array_ptr = call_in_use->vertex_array;
        sa_Log_Assert_Message_m(vertex_array_ptr, "Invalid pointer for vertex array");
        call_in_use->vertex_array_length = vertex_amount;
#if defined(SACI_DEBUG_MODE)
        if (!pos_array) {
            sa_Log_Error_Print_m(sa_LOG_TYPE_DEBUG, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL,
                                 "Invalid position array param");
        }

        if (!uv_array) {
            sa_Log_Error_Print_m(sa_LOG_TYPE_DEBUG, sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_OPENGL,
                                 "Null uv array param");
        }

        if (!color_array) {
            sa_Log_Error_Print_m(sa_LOG_TYPE_DEBUG, sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_OPENGL,
                                 "Null color array param");
        }
#endif
        for (sa_u32 i = 0; i < vertex_amount; ++i) {
            sa_uv uv = (!uv_array ? (sa_uv){0, 0} : uv_array[i]);
            sa_color color = (!color_array ? (sa_color){0, 0, 0, 0} : color_array[i]);
            vertex_array_ptr[i].pos = pos_array[i];
            vertex_array_ptr[i].uv = uv;
            vertex_array_ptr[i].color = color;
        }
    }
    { // Index operations
        sa_u32* bound_array = rendr->bound_index_array_buffer;
        sa_u32 bound_array_count = rendr->bound_index_array_length;

        sa_u32* index_array = call_in_use->index_array;
        call_in_use->index_array_length = bound_array_count;

        memcpy(index_array, bound_array, bound_array_count * sizeof(sa_u32));
    }
    ++rendr->call_in_use;
}

SA_API void sc_Renderer_End(struct __sc_renderer* rendr) {
    __sc_Renderer_Call_Array_Sort(rendr->call_array, rendr->call_in_use);
    __sc_Renderer_Batch_Calls(rendr);
    __sc_Renderer_Batch_Flush(rendr);
}

SA_API void sc_Renderer_Free(struct __sc_renderer* rendr) {
    sc_Renderer_Begin(rendr);
    __sc_Renderer_Free_Memory_m(rendr);
    __sc_Renderer_Free_OpenGL_m(rendr);
    free(rendr);
}

SA_API void sc_Renderer_Free_Opts(struct __sc_renderer* rendr, int free_opts) {
    sc_Renderer_Begin(rendr);
    if (free_opts & sc_RENDERER_FREE_OPT_MEMORY) {
        __sc_Renderer_Free_Memory_m(rendr);
    }
    if (free_opts & sc_RENDERER_FREE_OPT_OPENGL) {
        __sc_Renderer_Free_OpenGL_m(rendr);
    }
    free(rendr);
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

static sa_u32 __sc_shader_compile(const char* shaderSource, sa_u32 shaderType);

sa_u32 sc_Shader_Compile_Shader_Vert(const char* source) {
    return __sc_shader_compile(source, GL_VERTEX_SHADER);
}

sa_u32 sc_Shader_Compile_Shader_Frag(const char* source) {
    return __sc_shader_compile(source, GL_FRAGMENT_SHADER);
}

sa_u32 sc_Shader_Compile_Shader_Geom(const char* source) {
    return __sc_shader_compile(source, GL_GEOMETRY_SHADER);
}

sa_u32 sc_Shader_Create_Shader_Program(sa_u32 vshader, sa_u32 fshader) {
    sa_u32 programID = glCreateProgram();
    glAttachShader(programID, vshader);
    glAttachShader(programID, fshader);
    glLinkProgram(programID);

    sa_s32 success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char glErrMessage[1024];
        char errMessage[2048];
        int sizeReturned = 0;
        glGetProgramInfoLog(programID, 2048, &sizeReturned, glErrMessage);
        snprintf(errMessage, sizeof(errMessage), "Shader program couldn't be loaded: %s",
                 glErrMessage);
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, errMessage);
        return 0;
    }
    glDetachShader(programID, vshader);
    glDetachShader(programID, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    sa_Log_Info_Print_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_OPENGL,
                        "Shader program loaded successfully");
    return programID;
}

sa_u32 sc_Shader_Create_Shader_Program_Geom(sa_u32 vshader, sa_u32 fshader, sa_u32 gshader) {
    sa_u32 programID = glCreateProgram();
    glAttachShader(programID, vshader);
    glAttachShader(programID, fshader);
    glAttachShader(programID, gshader);
    glLinkProgram(programID);

    sa_s32 success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char glErrMessage[1024];
        char errMessage[2048];
        int sizeReturned = 0;
        glGetProgramInfoLog(programID, 2048, &sizeReturned, glErrMessage);
        snprintf(errMessage, sizeof(errMessage), "Shader program couldn't be loaded: %s",
                 glErrMessage);
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, errMessage);
        return 0;
    }
    glDetachShader(programID, vshader);
    glDetachShader(programID, fshader);
    glDetachShader(programID, gshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    glDeleteShader(gshader);
    sa_Log_Info_Print_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_OPENGL,
                        "Shader program be loaded successfully");

    return programID;
}

/* === GL Helper ===  */

static sa_u32 __sc_shader_compile(const char* shaderSource, sa_u32 shaderType) {
    sa_u32 shaderID = glCreateShader(shaderType);

    glShaderSource(shaderID, 1, &shaderSource, NULL);
    glCompileShader(shaderID);

    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char errMessage[2048];
        int sizeReturned = 0;
        glGetShaderInfoLog(shaderID, 2048, &sizeReturned, &errMessage[0]);

        glDeleteShader(shaderID);
        { // Logging
            char* logMessage = "";
            if (shaderType == GL_VERTEX_SHADER) {
                logMessage = "Vertex shader couldn't be loaded";
            }
            if (shaderType == GL_FRAGMENT_SHADER) {
                logMessage = "Fragment shader couldn't be loaded";
            }
            if (shaderType == GL_GEOMETRY_SHADER) {
                logMessage = "Geometry shader couldn't be loaded";
            }
            sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH,
                                 sa_LOG_CONTEXT_OPENGL, logMessage);
        }
        return 0;
    }
    {
        char* logMessage = "";
        if (shaderType == GL_VERTEX_SHADER) {
            logMessage = "Vertex shader loaded succesfully";
        }
        if (shaderType == GL_FRAGMENT_SHADER) {
            logMessage = "Fragment shader loaded succesfully";
        }
        if (shaderType == GL_GEOMETRY_SHADER) {
            logMessage = "Geometry shader loaded succesfully";
        }
        sa_Log_Info_Print_m(sa_LOG_TYPE_INFO,
                            sa_LOG_CONTEXT_OPENGL, logMessage);
    }

    return shaderID;
}
