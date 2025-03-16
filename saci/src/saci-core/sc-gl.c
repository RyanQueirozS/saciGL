#include <glad/glad.h>

#include "saci-core/sc-gl.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-math.h"
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

SA_API sa_bool_t sc_Event_Is_Key_Pressed(sc_window_t* window, int keycode) {
    return glfwGetKey(window, sa_SCAST_TO_m(int)(keycode)) == GLFW_PRESS;
}

/* === Renderer === */

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

// These might change and might not be used later...
#  define __SC_U_MODEL_MATRIX_LOC 0
#  define __SC_U_VIEW_MATRIX_LOC 1
#  define __SC_U_PROJECTION_MATRIX_LOC 2
#  define __SC_U_USE_CAM_LOC 3
#  define __SC_U_TEXTURE_LOC 4
#  define __SC_U_USE_TEXTURE_LOC 5

#endif // __SC_RENDERER_UNIFORMS

#ifndef __SC_RENDERER_VERT_SHADER
#  define __SC_RENDERER_VERT_SHADER
static const char* vert_shader =
    "#version 330 core\n"

    "layout (location = 0) in vec3 a_pos;\n"
    "layout (location = 1) in vec4 a_color;\n"
    "layout (location = 2) in vec2 a_texcoord;\n"

    "uniform mat4 u_model_matrix;\n"
    "uniform mat4 u_view_matrix;\n"
    "uniform mat4 u_projection_matrix;\n"
    "uniform bool u_use_cam;\n"

    "out vec4 v_color;\n"
    "out vec2 v_texcoord;\n"

    "void main()\n"
    "{\n"
    "   vec4 world_position = u_model_matrix * vec4(a_pos, 1.0);\n"
    "   if (u_use_cam) {\n"
    "       gl_Position = u_projection_matrix * u_view_matrix * world_position;\n"
    "   } else {\n"
    "       gl_Position = world_position;\n"
    "   }\n"
    "   v_color = a_color;\n"
    "   v_texcoord = a_texcoord;\n"
    "}\n\0";
#endif

#ifndef __SC_RENDERER_FRAG_SHADER
#  define __SC_RENDERER_FRAG_SHADER
static const char* frag_shader =
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
#endif

#ifndef SC_RENDERER_STRUCT
#  define SC_RENDERER_STRUCT

struct sc_renderer {
    sa_textureId current_texture_id;

    sa_u32_t* bound_index_array;
    sa_u32_t bound_index_array_count;
    sa_u32_t vertices_overlaped;

    sa_shaderId shader_program;
    sa_bufferId ibo;
    sa_bufferId vbo;
    sa_bufferId vao;

    struct __sc_batch {
        sa_u32_t index_array_count;
        sa_u32_t vertex_array_count;

        sa_u32_t* index_array;
        struct __sc_vertex {
            sa_vec3_t pos;
            sa_color_t color;
            sa_uv uv;
        }* vertex_array;
    } batch;
    sa_u32_t batch_count; // NOT CURRENTLY IN USE
};

#endif // SC_RENDERER_STRUCT

SA_API sc_renderer* sc_Renderer_New_Default(void) {
    sc_renderer* rendr = malloc(sizeof(sc_renderer));
    assert(rendr);
    { // Shader init
        sa_shaderId v_shader = sc_Shader_Compile_Shader_Vert(vert_shader);
        sa_shaderId f_shader = sc_Shader_Compile_Shader_Frag(frag_shader);
        assert(v_shader && f_shader);
        rendr->shader_program = sc_Shader_Create_Shader_Program(v_shader, f_shader);
        assert(rendr->shader_program);
    }
    { // VBO and IBO
        rendr->vbo = sc_GL_Create_Vertex_Buffer(sizeof(struct __sc_vertex) * 1000, NULL, GL_DYNAMIC_DRAW);

        rendr->ibo = sc_GL_Create_Index_Buffer_Dynamic(NULL, 1000);
    }

    { // VertexAttrib init
        sc_GL_Create_Vertex_Array(1, &rendr->vao);
        sc_GL_Bind_Vertex_Array(rendr->vao);

        sc_GL_Bind_Vertex_Buffer(rendr->vbo);

        sc_GL_Set_Vertex_Attrib_Pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct __sc_vertex),
                                        sa_SCAST_TO_m(void*) offsetof(struct __sc_vertex, pos));
        sc_GL_Enable_Vertex_Attrib_Array(0);
        sc_GL_Set_Vertex_Attrib_Pointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct __sc_vertex),
                                        sa_SCAST_TO_m(void*) offsetof(struct __sc_vertex, color));
        sc_GL_Enable_Vertex_Attrib_Array(1);
        sc_GL_Set_Vertex_Attrib_Pointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct __sc_vertex),
                                        sa_SCAST_TO_m(void*) offsetof(struct __sc_vertex, uv));
        sc_GL_Enable_Vertex_Attrib_Array(2);
    }
    return rendr;
}

SA_API void sc_Renderer_Begin(sc_renderer* rendr) {
    // Free boud index array
    if (rendr->bound_index_array) {
        free(rendr->bound_index_array);
        rendr->bound_index_array = NULL;
    }
    rendr->bound_index_array_count = 0;

    // Free batch index array
    if (rendr->batch.index_array) {
        free(rendr->batch.index_array);
        rendr->batch.index_array = NULL;
    }
    rendr->batch.index_array_count = 0;

    // Free batch vertex array
    if (rendr->batch.vertex_array) {
        free(rendr->batch.vertex_array);
        rendr->batch.vertex_array = NULL;
    }
    rendr->batch.vertex_array_count = 0;

    rendr->batch_count = 0;

    rendr->current_texture_id = 0;

    rendr->vertices_overlaped = 0;
}

SA_API void sc_Renderer_Bind_Texture(sc_renderer* rendr, sa_textureId tex_id) {
    rendr->current_texture_id = tex_id;
}

SA_API void sc_Renderer_Bind_Index_Buffer(sc_renderer* rendr, sa_u32_t* new_indices, sa_u32_t new_indices_count) {
    // Reset the index buffer
    if (rendr->bound_index_array)
        free(rendr->bound_index_array);
    rendr->bound_index_array_count = 0;

    // Separate to a simpler name
    sa_u32_t** index_array = &rendr->bound_index_array;
    sa_u32_t* index_count = &rendr->bound_index_array_count;

    sa_u32_t* new_array = (sa_u32_t*)sa_MALLOC(new_indices_count * sizeof(sa_u32_t));
    if (!new_array) {
        printf("Alloc error bind index\n");
        return;
    }

    // Copy the existing indices to the new array if it exists.
    if (*index_array) {
        memcpy(new_array, *index_array, (*index_count) * sizeof(sa_u32_t));
        free(*index_array);
    }

    memcpy(new_array + (*index_count), new_indices, new_indices_count * sizeof(sa_u32_t));

    *index_array = new_array;
    *index_count = new_indices_count;
}

SA_API void sc_Renderer_Push_Vertex(sc_renderer* rendr, sa_vec3_t* pos_array, sa_uv* uv_array, sa_color_t* color_array, sa_u32_t vertex_amount) {
    { // Vertex operations
        struct __sc_vertex** vertex_array_ptr = &rendr->batch.vertex_array;
        sa_u32_t* vertex_array_count = &rendr->batch.vertex_array_count;

        sa_u32_t new_vertex_count = (*vertex_array_count) + vertex_amount;

        // Reallocate the vertex array to accommodate the new size
        struct __sc_vertex* new_vertex_array = realloc(*vertex_array_ptr, new_vertex_count * sizeof(struct __sc_vertex));
        if (!new_vertex_array) {
            printf("Alloc error vertex\n");
            return;
        }

        // Update the renderer's batch vertex array and vertex count
        *vertex_array_ptr = new_vertex_array;
        *vertex_array_count = new_vertex_count;

        // Add the new vertices to the array
        for (sa_u32_t i = 0; i < vertex_amount; ++i) {
            sa_uv uv = (!uv_array ? (sa_uv){0, 0} : uv_array[i]);
            sa_color_t color = (!color_array ? (sa_color_t){0, 0, 0, 0} : color_array[i]);
            (*vertex_array_ptr)[(*vertex_array_count) - vertex_amount + i].pos = pos_array[i];
            (*vertex_array_ptr)[(*vertex_array_count) - vertex_amount + i].uv = uv;
            (*vertex_array_ptr)[(*vertex_array_count) - vertex_amount + i].color = color;
        }
    }
    { // Index operations
        if (!rendr->bound_index_array || !rendr->bound_index_array_count)
            return;

        sa_u32_t** index_array = &rendr->batch.index_array;
        sa_u32_t* index_array_count = &rendr->batch.index_array_count;

        sa_u32_t** bound_array = &rendr->bound_index_array;
        sa_u32_t* bound_array_count = &rendr->bound_index_array_count;

        sa_u32_t prev_index_array_count = *index_array_count;
        sa_u32_t new_index_count = prev_index_array_count + (*bound_array_count);

        sa_u32_t* new_index_array = realloc(*index_array, new_index_count * sizeof(sa_u32_t));
        if (!new_index_array) {
            printf("Alloc error index\n");
            return;
        }

        *index_array = new_index_array;
        *index_array_count = new_index_count;

        // Loops through the newly created index and adds the vertex amount.
        // This is done to fit in the indices with the new model in the vertex
        // buffer. So, for example, the amount + 0 index represent 0 in this new model
        for (sa_u32_t i = prev_index_array_count; i < new_index_count; ++i) {
            (*index_array)[i] = rendr->vertices_overlaped + (*bound_array)[i - prev_index_array_count];
        }
        rendr->vertices_overlaped += vertex_amount;
    }
}

SA_API void sc_Renderer_End(sc_renderer* rendr) {
    glUseProgram(rendr->shader_program);

    glBindVertexArray(rendr->vao);

    glBindBuffer(GL_ARRAY_BUFFER, rendr->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    sa_SCAST_TO_m(long int)(sizeof(struct __sc_vertex) * rendr->batch.vertex_array_count),
                    rendr->batch.vertex_array);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendr->ibo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                    sa_SCAST_TO_m(long int)(sizeof(sa_u32_t) * rendr->batch.index_array_count),
                    rendr->batch.index_array);

    { // Uniforms
        glEnable(GL_DEPTH_TEST);
        sa_mat4_t view = sa_Mat4_Look_At((sa_vec3_t){5.0f, 1.0f, 0.0f}, (sa_vec3_t){0.0f, 0.0f, 0.0f}, (sa_vec3_t){0.0f, 1.0f, 0.0f});
        sa_mat4_t projection = sa_Mat4_Perspective(90, 9.0 / 16, 1, 100);
        sa_mat4_t modelMatrix = sa_Mat4_Identity();
        glUniformMatrix4fv(__SC_U_VIEW_MATRIX_LOC, 1, GL_FALSE, &view.m_data[0][0]);
        glUniformMatrix4fv(__SC_U_PROJECTION_MATRIX_LOC, 1, GL_FALSE, &projection.m_data[0][0]);
        glUniformMatrix4fv(__SC_U_MODEL_MATRIX_LOC, 1, GL_FALSE, &modelMatrix.m_data[0][0]);
        glUniform1i(__SC_U_USE_CAM_LOC, sa_TRUE);
    }

    // if (call->m_texture_id != 0) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // }

    glDrawElements(GL_TRIANGLES, sa_SCAST_TO_m(int)(rendr->batch.index_array_count), GL_UNSIGNED_INT,
                   0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

SA_API void sc_Renderer_Free(sc_renderer* rendr) {
    sc_Renderer_Begin(rendr);
    free(rendr);
}

/* === OpenGL === */

SA_API void sc_GL_Resize_Vertex_Buffer(sa_u32_t vao_id, sa_u32_t vbo_id, sa_u64_t new_size) {
    glBindVertexArray(vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sa_SCAST_TO_m(sa_s64_t)(new_size), NULL, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

SA_API sa_u32_t sc_GL_Create_Index_Buffer_Dynamic(sa_u32_t* indices, sa_u64_t indice_amount) {
    sa_u32_t ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sa_SCAST_TO_m(sa_s64_t)(indice_amount * sizeof(sa_u32_t)), &indices[0],
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    assert(ibo);
    return ibo;
}

SA_API sa_u32_t sc_GL_Create_Index_Buffer_Static(sa_u32_t* indices, sa_u64_t indice_amount) {
    sa_u32_t ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sa_SCAST_TO_m(sa_s64_t)(indice_amount * sizeof(sa_u32_t)), &indices[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    assert(ibo);
    return ibo;
}

SA_API void sc_GL_Create_Vertex_Array(sa_u64_t size, sa_u32_t* arrays) {
    glGenVertexArrays(sa_SCAST_TO_m(int)(size), arrays);
}

SA_API void sc_GL_Bind_Vertex_Array(sa_u32_t array) {
    glBindVertexArray(array);
}

SA_API void sc_GL_Bind_Vertex_Buffer(sa_u32_t vbo) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

SA_API sa_u32_t sc_GL_Create_Vertex_Buffer(sa_u64_t size, const void* data, sa_u32_t usage) {
    sa_u32_t vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sa_SCAST_TO_m(long int)(size), data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vbo;
}

SA_API void sc_GL_Set_Vertex_Attrib_Pointer(sa_u32_t index, int size, sa_u32_t type, sa_bool_t normalized, sa_u64_t stride, void* ptr) {
    glVertexAttribPointer(index, size, type, normalized, sa_SCAST_TO_m(int)(stride), ptr);
}

void sc_GL_Enable_Vertex_Attrib_Array(sa_u32_t id) {
    glEnableVertexAttribArray(id);
}

/* === Shader Implementation === */

static sa_u32_t __sc_shader_compile(const char* shaderSource, sa_u32_t shaderType);

sa_u32_t sc_Shader_Compile_Shader_Vert(const char* source) {
    return __sc_shader_compile(source, GL_VERTEX_SHADER);
}

sa_u32_t sc_Shader_Compile_Shader_Frag(const char* source) {
    return __sc_shader_compile(source, GL_FRAGMENT_SHADER);
}

sa_u32_t sc_Shader_Compile_Shader_Geom(const char* source) {
    return __sc_shader_compile(source, GL_GEOMETRY_SHADER);
}

sa_u32_t sc_Shader_Create_Shader_Program(sa_u32_t vshader, sa_u32_t fshader) {
    sa_u32_t programID = glCreateProgram();
    glAttachShader(programID, vshader);
    glAttachShader(programID, fshader);
    glLinkProgram(programID);

    sa_s32_t success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char glErrMessage[1024];
        char errMessage[2048];
        int sizeReturned = 0;
        glGetProgramInfoLog(programID, 2048, &sizeReturned, glErrMessage);
        snprintf(errMessage, sizeof(errMessage), "Shader program couldn't be loaded: %s",
                 glErrMessage);
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, errMessage);
        return 0;
    }
    glDetachShader(programID, vshader);
    glDetachShader(programID, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_OPENGL,
                        "Shader program loaded successfully");
    return programID;
}

sa_u32_t sc_Shader_Create_Shader_Program_Geom(sa_u32_t vshader, sa_u32_t fshader, sa_u32_t gshader) {
    sa_u32_t programID = glCreateProgram();
    glAttachShader(programID, vshader);
    glAttachShader(programID, fshader);
    glAttachShader(programID, gshader);
    glLinkProgram(programID);

    sa_s32_t success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char glErrMessage[1024];
        char errMessage[2048];
        int sizeReturned = 0;
        glGetProgramInfoLog(programID, 2048, &sizeReturned, glErrMessage);
        snprintf(errMessage, sizeof(errMessage), "Shader program couldn't be loaded: %s",
                 glErrMessage);
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, errMessage);
        return 0;
    }
    glDetachShader(programID, vshader);
    glDetachShader(programID, fshader);
    glDetachShader(programID, gshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    glDeleteShader(gshader);
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_OPENGL,
                        "Shader program be loaded successfully");

    return programID;
}

// helper
static sa_u32_t __sc_shader_compile(const char* shaderSource, sa_u32_t shaderType) {
    sa_u32_t shaderID = glCreateShader(shaderType);

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
            sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH,
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
        sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO,
                            sa_LOG_CONTEXT_OPENGL, logMessage);
    }

    return shaderID;
}
