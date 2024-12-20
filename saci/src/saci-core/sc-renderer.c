#include <glad/glad.h>

#define ARENA_ALLOCATOR_IMPL
#include "arena.h"

#include "saci-core/sc-gl.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* === Structs for Helper Functions === */

typedef struct sc_RenderCall sc_RenderCall;
typedef struct sc_RenderBatch sc_RenderBatch;

/* === Helper Functions === */

void __sc_Renderer_Remove_Garbage_Numbers(sc_Renderer_t* renderer);

sc_RenderCall __sc_RenderCall_Create(Arena* arena, struct sc_Vertice_c* vertices, sa_u64_t vertices_amount,
                                     sa_u64_t indices_amount, int render_mode, sa_textureId tex_id,
                                     sa_u32_t ibo, sa_Mat4_t model_matrix);

void __sc_RenderBatch_Push(sc_RenderBatch* render_batch, sc_RenderCall render_call);

void __sc_Renderer_Init_GL_Vertex_Attrib_Context(sc_Renderer_t* renderer);

void __sc_Renderer_Init_Shader_Program(sc_Renderer_t* renderer);

void __sc_Renderer_Init_All(sc_Renderer_t* renderer);

void __sc_Renderer_Set_Uniform(sc_Renderer_t* renderer, const struct sc_Camera_c* camera,
                               sa_Mat4_t model_matrix, bool use_texture);

/* === Local Definitions === */

#define SACI_RENDER_BATCH_DEFAULT_CAPACITY 0x1000000 // TODO change name of this
#define SACI_DEFAULT_TEXTURE_BUFFER_SIZE 1           // TODO

static struct sc_RenderConfig {
    enum sc_Renderer_Projection_Mode_e m_projection_mode;
    sc_Renderer_ProjectionFunc m_custom_projection_func;

    bool m_should_fill_shape;
} __sc_renderer_config_s;

struct sc_Vertice_c {
    sa_Vec3_t m_pos;
    sa_Color_t m_color;
    sa_Vec2_t m_texcoord;
};

typedef struct sc_RenderCall {
    struct sc_Vertice_c* m_vertices;
    sa_u64_t m_vertice_amount;

    sa_u64_t m_indice_amount;
    sa_u32_t m_ibo;

    int m_render_mode; // LINE TRIANGLE or QUAD
    sa_textureId m_texture_id;

    sa_Mat4_t m_model_matrix;
} sc_RenderCall;

typedef struct sc_RenderBatch {
    sc_RenderCall* m_render_calls;
    sa_u32_t m_capacity;
    sa_u32_t m_render_call_count;
} sc_RenderBatch;

// TODO doc
struct sc_Renderer_c {
    sa_u32_t m_vao, m_vbo;
    sa_u32_t m_shader_program;

    Arena m_memory_context;
    sc_RenderBatch m_render_batch;
};

struct sc_ModelMesh_c {
    struct sc_Vertice_c* m_vertices;
    sa_u64_t m_vertices_amount;

    sa_u32_t* m_indices;
    sa_u64_t m_indices_amount;
    sa_u32_t m_ibo;
};

/* === Renderer Implementation === */

sa_Vec3_t sc_Vertice_Get_Pos(const struct sc_Vertice_c* vertice) {
    return vertice->m_pos;
}

sa_Color_t sc_Vertice_Get_Color(const struct sc_Vertice_c* vertice) {
    return vertice->m_color;
}

sa_Vec2_t sc_Vertice_Get_Texcoord(const struct sc_Vertice_c* vertice) {
    return vertice->m_texcoord;
}

struct sc_Vertice_c* sc_Vertice_Create_Vertice(sa_Vec3_t position, sa_Color_t color, sa_Vec2_t texcood) {
    struct sc_Vertice_c* vertice = (struct sc_Vertice_c*)malloc(sizeof(struct sc_Vertice_c));
    vertice->m_pos = position;
    vertice->m_color = color;
    vertice->m_texcoord = texcood;
    return vertice;
}

void sc_Vertice_Get_Array_Info(struct sc_Vertice_c* vertex_array, sa_u64_t vertex_array_size_out,
                               sa_Vec3_t** positions_out,
                               sa_Color_t** colors_out,
                               sa_Vec2_t** texcoords_out) {
    if (!vertex_array) {
        // LOG TODO
        return;
    }
    *positions_out = (sa_Vec3_t*)malloc(sizeof(sa_Vec3_t) * vertex_array_size_out);
    *colors_out = (sa_Color_t*)malloc(sizeof(sa_Color_t) * vertex_array_size_out);
    *texcoords_out = (sa_Vec2_t*)malloc(sizeof(sa_Vec2_t) * vertex_array_size_out);
    // TODO check errors

    for (sa_u64_t i = 0; i < vertex_array_size_out; ++i) {
        struct sc_Vertice_c vertex = vertex_array[i];
        (*positions_out)[i] = vertex.m_pos;
        (*colors_out)[i] = vertex.m_color;
        (*texcoords_out)[i] = vertex.m_texcoord;
    }
}

struct sc_Vertice_c* sc_Vertice_Create_Vertices_Array(sa_Vec3_t* positions,
                                                      sa_Color_t* colors,
                                                      sa_Vec2_t* texcoords,
                                                      sa_u64_t amount) {
    struct sc_Vertice_c* vertices = (struct sc_Vertice_c*)malloc(sizeof(struct sc_Vertice_c) * amount);
    if (!vertices) {
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                             "Could not allocate new vertices");
        return NULL;
    }
    for (sa_u64_t i = 0; i < amount; ++i) {
        if (positions)
            vertices[i].m_pos = positions[i];
        if (colors)
            vertices[i].m_color = colors[i];
        if (texcoords)
            vertices[i].m_texcoord = texcoords[i];
    }
    return vertices;
}

struct sc_Vertice_c* sc_ModelMesh_Get_Vertices(const struct sc_ModelMesh_c* modelMesh) {
    return modelMesh->m_vertices;
}

sa_u64_t sc_ModelMesh_Get_Vertices_Amount(const struct sc_ModelMesh_c* modelMesh) {
    return modelMesh->m_vertices_amount;
}

sa_u32_t* sc_ModelMesh_Get_Indices(const struct sc_ModelMesh_c* modelMesh) {
    return modelMesh->m_indices;
}

sa_u64_t sc_ModelMesh_GetIndicesAmount(const struct sc_ModelMesh_c* modelMesh) {
    return modelMesh->m_indices_amount;
}

sc_Renderer_t* sc_Renderer_Create_Empty() {
    sc_Renderer_t* renderer = (sc_Renderer_t*)malloc(sizeof(sc_Renderer_t));
    if (!renderer) {
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER,
                             "Renderer could not be initialized");
        return NULL;
    }
    __sc_Renderer_Remove_Garbage_Numbers(renderer);
    return renderer;
}

sc_Renderer_t* sc_Renderer_Create_Default() {
    sc_Renderer_t* renderer = sc_Renderer_Create_Empty();
    if (!renderer) {
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER,
                             "Renderer could not be initialized");
        return NULL;
    }
    sc_Renderer_Init_Memory_Context(renderer, SACI_RENDER_BATCH_DEFAULT_CAPACITY);

    __sc_Renderer_Init_All(renderer);
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer created successfully");
    return renderer;
}

void sc_Renderer_Init_Memory_Context(sc_Renderer_t* renderer, sa_u64_t size) {
    ArenaInit(&renderer->m_memory_context, size);
}

void sc_Renderer_Resize_Render_Buffer(sc_Renderer_t* renderer, sa_u64_t newSize) {
    sc_RenderBatch* renderBatch = &renderer->m_render_batch;
    if (newSize <= 0 || newSize <= renderBatch->m_render_call_count) {
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER,
                             "RenderBatch new size is not valid");
        return;
    }
    sc_RenderCall* newRenderCalls = (sc_RenderCall*)malloc(newSize * sizeof(sc_RenderCall));
    if (!newRenderCalls) {
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER,
                             "RenderBatch Couldn't create new render call");
        return;
    }
    memcpy(newRenderCalls, renderBatch->m_render_calls, renderBatch->m_render_call_count * sizeof(sc_RenderCall));
    free(renderBatch->m_render_calls);
    renderBatch->m_render_calls = newRenderCalls;
    renderBatch->m_capacity = newSize;
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "RenderBatch resized successfully");
    assert(renderer->m_render_batch.m_render_calls); // TODO SACI_ASSERT
}

void sc_Renderer_Delete(sc_Renderer_t* renderer) {
    if (!renderer) {
        // TODO
        // create log
        return;
    }
    sc_Renderer_Begin(renderer);
    ArenaDelete(&renderer->m_memory_context);
    free(renderer->m_render_batch.m_render_calls);

    glDeleteVertexArrays(1, &renderer->m_vao);
    glDeleteProgram(renderer->m_shader_program);

    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer deleted successfully");
}

void sc_Renderer_Set_No_Fill_Mode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    __sc_renderer_config_s.m_should_fill_shape = sa_FALSE;
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer set no fill mode");
}

void sc_Renderer_Set_Fill_Mode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    __sc_renderer_config_s.m_should_fill_shape = sa_TRUE;
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer set fill mode");
}

void sc_Renderer_Enable_Z_Buffer(void) {
    glEnable(GL_DEPTH_TEST);
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer enabled Z buffer");
}

void sc_Renderer_Set_Projection_Mode(enum sc_Renderer_Projection_Mode_e renderProjectionMode) {
    __sc_renderer_config_s.m_projection_mode = renderProjectionMode;
    switch (renderProjectionMode) {
    case sa_RENDERER_PROJECTION_MODE_ORTHO: {
        sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                            "Renderer set projection mode to ORTHOGRAPHIC");
        break;
    }
    case sa_RENDERER_PROJECTION_MODE_PERSPECTIVE: {
        sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                            "Renderer set projection mode to PERSPECTIVE");
        break;
    }
    case sa_RENDERER_PROJECTION_MODE_CUSTOM: {
        sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                            "Renderer set projection mode to CUSTOM");
        break;
    }
    }
}

void sc_Renderer_SetCustom_Projection_Function(
    sc_Renderer_ProjectionFunc renderCustomProjectionMode) {
    __sc_renderer_config_s.m_custom_projection_func = renderCustomProjectionMode;
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer set custom projection mode function");
}

void sc_Renderer_Begin(sc_Renderer_t* renderer) {
    ArenaReset(&renderer->m_memory_context);
    renderer->m_render_batch.m_render_call_count = 0;
}

void sc_Renderer_End(sc_Renderer_t* renderer, const struct sc_Camera_c* camera) {
    glUseProgram(renderer->m_shader_program);

    glBindVertexArray(renderer->m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->m_vbo);

    for (sa_u32_t i = 0; i < renderer->m_render_batch.m_render_call_count; ++i) {
        sc_RenderCall* call = &renderer->m_render_batch.m_render_calls[i];
        __sc_Renderer_Set_Uniform(renderer, camera, call->m_model_matrix,
                                  call->m_texture_id);

        if (call->m_texture_id != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, call->m_texture_id);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, call->m_ibo);

        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        sizeof(struct sc_Vertice_c) * call->m_vertice_amount,
                        call->m_vertices);

        glDrawElements(call->m_render_mode, call->m_indice_amount, GL_UNSIGNED_INT,
                       0);

        if (call->m_texture_id != 0) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    glBindVertexArray(0);
    glUseProgram(0);
}

void sc_Renderer_Push_Vertices(sc_Renderer_t* renderer, struct sc_Vertice_c* vertices,
                               sa_u64_t verticeAmount,
                               sa_u64_t indiceAmount, sa_Mat4_t modelMatrix,
                               sa_textureId texID, sa_u32_t ibo) {
    if (!vertices) {
        sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER, "Invalid vertices");
        return;
    }
    sc_RenderCall renderCall = __sc_RenderCall_Create(&renderer->m_memory_context,
                                                      vertices, verticeAmount,
                                                      indiceAmount, GL_TRIANGLES,
                                                      texID, ibo, modelMatrix);
    __sc_RenderBatch_Push(&renderer->m_render_batch, renderCall);
}

void sc_Renderer_Push_Model_Mesh(sc_Renderer_t* renderer, struct sc_ModelMesh_c* mesh,
                                 sa_Mat4_t modelMatrix, sa_textureId texID) {
    if (!mesh) {
        exit(1);
    }
    sc_Renderer_Push_Vertices(renderer, mesh->m_vertices, mesh->m_vertices_amount,
                              mesh->m_indices_amount, modelMatrix,
                              texID, mesh->m_ibo);
}

struct sc_ModelMesh_c* sc_ModelMesh_Create(sa_Vec3_t* verticesPos,
                                           sa_u64_t verticePosAmount,
                                           sa_Vec2_t* verticesTexcoord,
                                           sa_u64_t verticesTexcoordAmount,
                                           struct sc_VertexIndice_c* indices,
                                           sa_u64_t indiceAmount) {
    struct sc_ModelMesh_c* mesh = (struct sc_ModelMesh_c*)malloc(sizeof(struct sc_ModelMesh_c));
    if (!mesh) {
        return NULL;
    }

    assert(verticePosAmount < SACI_RENDER_BATCH_DEFAULT_CAPACITY);

    mesh->m_vertices_amount = verticePosAmount;
    mesh->m_indices_amount = indiceAmount;

    mesh->m_vertices =
        (struct sc_Vertice_c*)malloc(sizeof(struct sc_Vertice_c) * mesh->m_vertices_amount);
    if (!mesh->m_vertices) {
        free(mesh);
        return NULL;
    }

    for (sa_u64_t i = 0; i < verticePosAmount; i++) {
        float r =
            (rand() % 10001) / 10000.0f; // Generates a float between 0 and 1
        float g = (rand() % 10001) / 10000.0f;
        float b = (rand() % 10001) / 10000.0f;
        mesh->m_vertices[i].m_pos = verticesPos[i];
        mesh->m_vertices[i].m_texcoord = verticesTexcoord[i];
        mesh->m_vertices[i].m_color =
            (sa_Color_t){r, g, b, 1.0f}; // Default white color
    }

    mesh->m_indices = (sa_u32_t*)malloc(sizeof(sa_u32_t) * mesh->m_indices_amount);
    if (!mesh->m_indices) {
        free(mesh->m_vertices);
        free(mesh);
        return NULL;
    }

    for (sa_u64_t i = 0; i < indiceAmount; i++) {
        mesh->m_indices[i] = indices[i].vertexIndex;
    }

    mesh->m_ibo = sc_GL_Create_Index_Buffer(mesh->m_indices, indiceAmount);

    free(verticesPos);
    free(verticesTexcoord);
    free(indices);

    return mesh;
}

struct sc_ModelMesh_c* sc_Model_Mesh_Load(const char* path, sc_OBJ_File_Reading_Function fileReader) {
    sa_Vec3_t* verticesPos = NULL;
    sa_u64_t verticesAmount = 0;
    sa_Vec2_t* verticesTexCoords = NULL;
    sa_u64_t verticesTexCoordsAmount = 0;
    struct sc_VertexIndice_c* indices = NULL;
    sa_u64_t indicesAmount = 0;

    if (!sc_OBJ_Parse(path, fileReader, &verticesPos, &verticesAmount,
                      &verticesTexCoords, &verticesTexCoordsAmount, &indices,
                      &indicesAmount)) {
        return NULL;
    }
    return sc_ModelMesh_Create(verticesPos, verticesAmount, verticesTexCoords,
                               verticesTexCoordsAmount, indices, indicesAmount);
}

void sc_Model_Mesh_Delete(struct sc_ModelMesh_c* modelMesh) {
    if (!modelMesh)
        return;
    if (modelMesh->m_indices)
        free(modelMesh->m_indices);
    if (modelMesh->m_vertices)
        free(modelMesh->m_vertices);
    if (modelMesh->m_ibo)
        glDeleteBuffers(1, &modelMesh->m_ibo);
    free(modelMesh);
}

/* === Helper Implementation === */

void __sc_Renderer_Remove_Garbage_Numbers(sc_Renderer_t* renderer) {
    renderer->m_render_batch.m_render_calls = NULL;
    renderer->m_render_batch.m_render_call_count = 0;
    renderer->m_render_batch.m_capacity = 0;
    renderer->m_vao = 0;
    renderer->m_vbo = 0;
}

void __sc_Renderer_Init_All(sc_Renderer_t* renderer) {
    sc_Renderer_Resize_Render_Buffer(renderer, SACI_RENDER_BATCH_DEFAULT_CAPACITY);

    __sc_Renderer_Init_GL_Vertex_Attrib_Context(renderer);
    __sc_Renderer_Init_Shader_Program(renderer);
#if defined(SACI_DEBUG_MODE) || defined(SACI_DEBUG_MODE_RENDERING)
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer initialized successfully");
#endif
}

sc_RenderCall __sc_RenderCall_Create(Arena* arena, struct sc_Vertice_c* vertices, sa_u64_t verticesAmount,
                                     sa_u64_t indicesAmount,
                                     int renderMode, sa_textureId texID, sa_u32_t ibo,
                                     sa_Mat4_t modelMatrix) {
    sa_ASSERT(arena);
    sa_ASSERT(vertices);

    struct sc_Vertice_c* arenaVertices = ArenaAlloc(arena, sizeof(struct sc_Vertice_c) * verticesAmount);
    sa_ASSERT(arenaVertices);

    memcpy(arenaVertices, vertices, sizeof(struct sc_Vertice_c) * verticesAmount);

    sc_RenderCall renderCall = {
        .m_vertices = arenaVertices,
        .m_vertice_amount = verticesAmount,

        .m_indice_amount = indicesAmount,

        .m_render_mode = renderMode,
        .m_texture_id = texID,
        .m_ibo = ibo,

        .m_model_matrix = modelMatrix,
    };
    return renderCall;
}

void __sc_RenderBatch_Push(sc_RenderBatch* renderBatch,
                           sc_RenderCall renderCall) {
    if (renderBatch->m_capacity <= renderBatch->m_render_call_count) {
        // todo add option to resize after size reached
        sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                            "RenderBatch RenderCount exceeded Capacity ");
        return;
    }
    renderBatch->m_render_calls[renderBatch->m_render_call_count] = renderCall;
    renderBatch->m_render_call_count++;
}

void __sc_Renderer_Init_GL_Vertex_Attrib_Context(sc_Renderer_t* renderer) {
    sc_GL_Create_Vertex_Array(1, &renderer->m_vao);
    sc_GL_Bind_Vertex_Array(renderer->m_vao);

    renderer->m_vbo = sc_GL_Create_Vertex_Buffer(renderer->m_render_batch.m_capacity * sizeof(struct sc_Vertice_c), NULL, GL_DYNAMIC_DRAW);
    sc_GL_Bind_Vertex_Buffer(renderer->m_vbo);

    sc_GL_Set_Vertex_Attrib_Pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct sc_Vertice_c),
                                    (void*)offsetof(struct sc_Vertice_c, m_pos));
    sc_GL_Enable_Vertex_Attrib_Array(0);
    sc_GL_Set_Vertex_Attrib_Pointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct sc_Vertice_c),
                                    (void*)offsetof(struct sc_Vertice_c, m_color));
    sc_GL_Enable_Vertex_Attrib_Array(1);
    sc_GL_Set_Vertex_Attrib_Pointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct sc_Vertice_c),
                                    (void*)offsetof(struct sc_Vertice_c, m_texcoord));
    sc_GL_Enable_Vertex_Attrib_Array(2);
}

void __sc_Renderer_Init_Shader_Program(sc_Renderer_t* renderer) {
    const char* vShaderSource =
        "#version 330 core\n"

        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec4 aColor;\n"
        "layout (location = 2) in vec2 aTexCoord;\n"

        "uniform mat4 uModelMatrix;\n"
        "uniform mat4 uViewMatrix;\n"
        "uniform mat4 uProjectionMatrix;\n"
        "uniform bool uUseCam;\n"

        "out vec4 vColor;\n"
        "out vec2 vTexCoord;\n"

        "void main()\n"
        "{\n"
        "   vec4 worldPosition = uModelMatrix * vec4(aPos, 1.0);\n"
        "   if (uUseCam) {\n"
        "       gl_Position = uProjectionMatrix * uViewMatrix * "
        "worldPosition;\n"
        "   } else {\n"
        "       gl_Position = worldPosition;\n"
        "   }\n"
        "   vColor = aColor;\n"
        "   vTexCoord = aTexCoord;\n"
        "}\n\0";

    const char* fShaderSource =
        "#version 330 core\n"

        "in vec4 vColor;\n"
        "in vec2 vTexCoord;\n"

        "uniform sampler2D uTexture;\n"
        "uniform bool uUseTexture;\n"

        "out vec4 FragColor;\n"

        "void main()\n"
        "{\n"
        "   if (uUseTexture) {\n"
        "       vec4 texColor = texture(uTexture, vTexCoord);\n"
        "       FragColor = texColor * vColor;\n"
        "   } else {\n"
        "       FragColor = vColor;\n"
        "   }\n"
        "}\n\0";

    sa_u32_t vShader = sc_Shader_Compile_Shader_V(vShaderSource);
    sa_u32_t fShader = sc_Shader_Compile_Shader_F(fShaderSource);
    assert(vShader != 0 && fShader != 0);
    renderer->m_shader_program = sc_Shader_Create_Shader_Program(vShader, fShader);
    assert(renderer->m_shader_program);
}

void __sc_Renderer_Set_Uniform(sc_Renderer_t* renderer, const struct sc_Camera_c* camera,
                               sa_Mat4_t modelMatrix, bool useTexture) {
    sa_Mat4_t view = {0};
    sa_Mat4_t projection = {0};

    int viewLoc = glGetUniformLocation(renderer->m_shader_program, "uViewMatrix");
    int projLoc = glGetUniformLocation(renderer->m_shader_program, "uProjectionMatrix");
    int useCamLoc = glGetUniformLocation(renderer->m_shader_program, "uUseCam");
    int uModelMatrixLoc = glGetUniformLocation(renderer->m_shader_program, "uModelMatrix");
    int uTextureLoc = glGetUniformLocation(renderer->m_shader_program, "uTexture");
    int uUseTextureLoc = glGetUniformLocation(renderer->m_shader_program, "uUseTexture");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view.m[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection.m[0][0]);
    glUniformMatrix4fv(uModelMatrixLoc, 1, GL_FALSE, &modelMatrix.m[0][0]);
    glUniform1i(useCamLoc, sa_TRUE);

    glUniform1i(uTextureLoc, 0);
    glUniform1i(uUseTextureLoc, sa_FALSE);

    if (useTexture) {
        glUniform1i(uUseTextureLoc, sa_TRUE);
    }

    if (camera == NULL) {
        glUniform1i(useCamLoc, sa_FALSE);
        return;
    }

    view = sa_Mat4_Look_At(camera->m_position, camera->m_target, camera->m_up);

    switch (__sc_renderer_config_s.m_projection_mode) {
    case sa_RENDERER_PROJECTION_MODE_ORTHO: {
        projection = sa_Mat4_Ortho(-1, 1, -1, 1, camera->m_near, camera->m_far);
        break;
    }
    case sa_RENDERER_PROJECTION_MODE_PERSPECTIVE: {
        projection = sa_Mat4_Perspective(camera->m_fov, camera->m_aspect_ratio,
                                         camera->m_near, camera->m_far);
        break;
    }
    case sa_RENDERER_PROJECTION_MODE_CUSTOM: {
        if (__sc_renderer_config_s.m_custom_projection_func == NULL) {
            sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                                "Custom projection mode function not set");
            return;
        }
        projection = __sc_renderer_config_s.m_custom_projection_func(*camera);
        break;
    }
    }
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view.m[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection.m[0][0]);
}
