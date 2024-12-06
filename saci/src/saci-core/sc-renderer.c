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

void __sc_Renderer_RemoveGarbageNumbers(sc_Renderer_t* renderer);

sc_RenderCall __sc_RenderCall_Create(Arena* arena, struct sc_Vertice_c* vertices, sa_U64_t verticesAmount,
                                     sa_U64_t indicesAmount, int renderMode, sa_Texture_ID texID,
                                     sa_U32_t ibo, sa_Mat4_t modelMatrix);

void __sc_RenderBatch_Push(sc_RenderBatch* renderBatch, sc_RenderCall renderCall);

void __sc_RenderBatch_Empty(sc_RenderBatch* renderBatch);

void __sc_RenderBatch_Free(sc_RenderBatch* renderBatch);

void __sc_Renderer_InitGLVertexAttribContext(sc_Renderer_t* renderer);

void __sc_Renderer_InitShaderProgram(sc_Renderer_t* renderer);

void __sc_Renderer_InitAll(sc_Renderer_t* renderer);

void __sc_Renderer_SetUniform(sc_Renderer_t* renderer, const struct sc_Camera_c* camera,
                              sa_Mat4_t modelMatrix, bool useTexture);

/* === Local Definitions === */

#define SACI_RENDER_BATCH_DEFAULT_CAPACITY 0x1000000
#define SACI_DEFAULT_TEXTURE_BUFFER_SIZE 1 // TODO

struct sc_Vertice_c {
    sa_Vec3_t pos;
    sa_Color_t color;
    sa_Vec2_t texCoord;
};

typedef struct sc_RenderCall {
    struct sc_Vertice_c* vertices;
    sa_U64_t verticeAmount;

    sa_U64_t indiceAmount;

    int renderMode; // LINE TRIANGLE or QUAD
    sa_Texture_ID textureID;
    sa_U32_t ibo;

    sa_Mat4_t modelMatrix;
} sc_RenderCall;

typedef struct sc_RenderBatch {
    sc_RenderCall* renderCalls;
    sa_U32_t capacity;
    sa_U32_t renderCallCount;
} sc_RenderBatch;

// TODO doc
struct sc_Renderer {
    sa_U32_t vao, vbo;
    sa_U32_t shaderProgram;

    Arena memoryContext;
    sc_RenderBatch renderBatch;
};

static struct sc_RenderConfig {
    enum sc_Renderer_Projection_Mode_e projectionMode;
    sc_Renderer_Custom_Projection_Func customProjectionFunction;

    bool shouldFillShape;
} sc_RenderConfig;

struct sc_ModelMesh_c {
    struct sc_Vertice_c* vertices;
    sa_U64_t verticesAmount;

    sa_U32_t* indices;
    sa_U64_t indicesAmount;
    sa_U32_t ibo;
};

/* === Renderer Implementation === */

sa_Vec3_t sc_Vertice_GetPos(const struct sc_Vertice_c* vertice) {
    return vertice->pos;
}

sa_Color_t sc_Vertice_GetColor(const struct sc_Vertice_c* vertice) {
    return vertice->color;
}

sa_Vec2_t sc_Vertice_GetTexcoord(const struct sc_Vertice_c* vertice) {
    return vertice->texCoord;
}

struct sc_Vertice_c* sc_Vertice_CreateVertice(sa_Vec3_t position, sa_Color_t color, sa_Vec2_t texcood) {
    struct sc_Vertice_c* vertice = (struct sc_Vertice_c*)malloc(sizeof(struct sc_Vertice_c));
    vertice->pos = position;
    vertice->color = color;
    vertice->texCoord = texcood;
    return vertice;
}

void sc_Vertice_GetArrayInfo(struct sc_Vertice_c* vertexArray, sa_U64_t vertexArraySize,
                             sa_Vec3_t** positions,
                             sa_Color_t** colors,
                             sa_Vec2_t** texcoords) {
    if (!vertexArray) {
        // LOG TODO
        return;
    }
    *positions = (sa_Vec3_t*)malloc(sizeof(sa_Vec3_t) * vertexArraySize);
    *colors = (sa_Color_t*)malloc(sizeof(sa_Color_t) * vertexArraySize);
    *texcoords = (sa_Vec2_t*)malloc(sizeof(sa_Vec2_t) * vertexArraySize);
    // TODO check errors

    for (sa_U64_t i = 0; i < vertexArraySize; ++i) {
        struct sc_Vertice_c vertex = vertexArray[i];
        (*positions)[i] = vertex.pos;
        (*colors)[i] = vertex.color;
        (*texcoords)[i] = vertex.texCoord;
    }
}

struct sc_Vertice_c* sc_Vertice_CreateVerticesArray(sa_Vec3_t* positions,
                                                    sa_Color_t* colors,
                                                    sa_Vec2_t* texcoords,
                                                    sa_U64_t amount) {
    struct sc_Vertice_c* vertices = (struct sc_Vertice_c*)malloc(sizeof(struct sc_Vertice_c) * amount);
    if (!vertices) {
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                             "Could not allocate new vertices");
        return NULL;
    }
    for (sa_U64_t i = 0; i < amount; ++i) {
        if (positions)
            vertices[i].pos = positions[i];
        if (colors)
            vertices[i].color = colors[i];
        if (texcoords)
            vertices[i].texCoord = texcoords[i];
    }
    return vertices;
}

struct sc_Vertice_c* sc_ModelMesh_GetVertices(const struct sc_ModelMesh_c* modelMesh) {
    return modelMesh->vertices;
}

sa_U64_t sc_ModelMesh_GetVerticesAmount(const struct sc_ModelMesh_c* modelMesh) {
    return modelMesh->verticesAmount;
}

sa_U32_t* sc_ModelMesh_GetIndices(const struct sc_ModelMesh_c* modelMesh) {
    return modelMesh->indices;
}

sa_U64_t sc_ModelMesh_GetIndicesAmount(const struct sc_ModelMesh_c* modelMesh) {
    return modelMesh->indicesAmount;
}

sc_Renderer_t* sc_Renderer_CreateEmpty() {
    sc_Renderer_t* renderer = (sc_Renderer_t*)malloc(sizeof(sc_Renderer_t));
    if (!renderer) {
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER,
                             "Renderer could not be initialized");
        return NULL;
    }
    __sc_Renderer_RemoveGarbageNumbers(renderer);
    return renderer;
}

sc_Renderer_t* sc_Renderer_CreateDefault() {
    sc_Renderer_t* renderer = sc_Renderer_CreateEmpty();
    if (!renderer) {
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_RENDERER,
                             "Renderer could not be initialized");
        return NULL;
    }
    sc_Renderer_InitMemoryContext(renderer, SACI_RENDER_BATCH_DEFAULT_CAPACITY);

    __sc_Renderer_InitAll(renderer);
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer created successfully");
    return renderer;
}

void sc_Renderer_InitMemoryContext(sc_Renderer_t* renderer, sa_U64_t size) {
    ArenaInit(&renderer->memoryContext, size);
}

void sc_Renderer_ResizeRenderBuffer(sc_Renderer_t* renderer, sa_U64_t newSize) {
    sc_RenderBatch* renderBatch = &renderer->renderBatch;
    if (newSize <= 0 || newSize <= renderBatch->renderCallCount) {
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
    memcpy(newRenderCalls, renderBatch->renderCalls, renderBatch->renderCallCount * sizeof(sc_RenderCall));
    free(renderBatch->renderCalls);
    renderBatch->renderCalls = newRenderCalls;
    renderBatch->capacity = newSize;
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "RenderBatch resized successfully");
    assert(renderer->renderBatch.renderCalls); // TODO SACI_ASSERT
}

void sc_Renderer_Delete(sc_Renderer_t* renderer) {
    if (!renderer) {
        // TODO
        // create log
        return;
    }
    sc_Renderer_Begin(renderer);
    ArenaDelete(&renderer->memoryContext);
    free(renderer->renderBatch.renderCalls);

    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteProgram(renderer->shaderProgram);

    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer deleted successfully");
}

void sc_Renderer_SetNoFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    sc_RenderConfig.shouldFillShape = sa_FALSE;
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer set no fill mode");
}

void sc_Renderer_SetFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    sc_RenderConfig.shouldFillShape = sa_TRUE;
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer set fill mode");
}

void sc_Renderer_EnableZBuffer(void) {
    glEnable(GL_DEPTH_TEST);
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer enabled Z buffer");
}

void sc_Renderer_SetProjectionMode(enum sc_Renderer_Projection_Mode_e renderProjectionMode) {
    sc_RenderConfig.projectionMode = renderProjectionMode;
    switch (renderProjectionMode) {
    case sa_RENDER_ORTHOGRAPHIC_PROJECTION: {
        sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                            "Renderer set projection mode to ORTHOGRAPHIC");
        break;
    }
    case sa_RENDER_PERSPECTIVE_PROJECTION: {
        sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                            "Renderer set projection mode to PERSPECTIVE");
        break;
    }
    case sa_RENDER_CUSTOM_PROJECTION: {
        sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                            "Renderer set projection mode to CUSTOM");
        break;
    }
    }
}

void sc_Renderer_SetCustomProjectionModeFunction(
    sc_Renderer_Custom_Projection_Func renderCustomProjectionMode) {
    sc_RenderConfig.customProjectionFunction = renderCustomProjectionMode;
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer set custom projection mode function");
}

void sc_Renderer_Begin(sc_Renderer_t* renderer) {
    ArenaReset(&renderer->memoryContext);
    renderer->renderBatch.renderCallCount = 0;
}

void sc_Renderer_End(sc_Renderer_t* renderer, const struct sc_Camera_c* camera) {
    glUseProgram(renderer->shaderProgram);

    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

    for (sa_U32_t i = 0; i < renderer->renderBatch.renderCallCount; ++i) {
        sc_RenderCall* call = &renderer->renderBatch.renderCalls[i];
        __sc_Renderer_SetUniform(renderer, camera, call->modelMatrix,
                                 call->textureID);

        if (call->textureID != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, call->textureID);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, call->ibo);

        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        sizeof(struct sc_Vertice_c) * call->verticeAmount,
                        call->vertices);

        glDrawElements(call->renderMode, call->indiceAmount, GL_UNSIGNED_INT,
                       0);

        if (call->textureID != 0) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    glBindVertexArray(0);
    glUseProgram(0);
}

void sc_Renderer_PushVertices(sc_Renderer_t* renderer, struct sc_Vertice_c* vertices,
                              sa_U64_t verticeAmount,
                              sa_U64_t indiceAmount, sa_Mat4_t modelMatrix,
                              sa_Texture_ID texID, sa_U32_t ibo) {
    if (!vertices) {
        sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER, "Invalid vertices");
        return;
    }
    sc_RenderCall renderCall = __sc_RenderCall_Create(&renderer->memoryContext,
                                                      vertices, verticeAmount,
                                                      indiceAmount, GL_TRIANGLES,
                                                      texID, ibo, modelMatrix);
    __sc_RenderBatch_Push(&renderer->renderBatch, renderCall);
}

void sc_Renderer_PushModelMesh(sc_Renderer_t* renderer, struct sc_ModelMesh_c* mesh,
                               sa_Mat4_t modelMatrix, sa_Texture_ID texID) {
    if (!mesh) {
        exit(1);
    }
    sc_Renderer_PushVertices(renderer, mesh->vertices, mesh->verticesAmount,
                             mesh->indicesAmount, modelMatrix,
                             texID, mesh->ibo);
}

struct sc_ModelMesh_c* sc_ModelMesh_Create(sa_Vec3_t* verticesPos,
                                           sa_U64_t verticePosAmount,
                                           sa_Vec2_t* verticesTexcoord,
                                           sa_U64_t verticesTexcoordAmount,
                                           struct sc_VertexIndice* indices,
                                           sa_U64_t indiceAmount) {
    struct sc_ModelMesh_c* mesh = (struct sc_ModelMesh_c*)malloc(sizeof(struct sc_ModelMesh_c));
    if (!mesh) {
        return NULL;
    }

    assert(verticePosAmount < SACI_RENDER_BATCH_DEFAULT_CAPACITY);

    mesh->verticesAmount = verticePosAmount;
    mesh->indicesAmount = indiceAmount;

    mesh->vertices =
        (struct sc_Vertice_c*)malloc(sizeof(struct sc_Vertice_c) * mesh->verticesAmount);
    if (!mesh->vertices) {
        free(mesh);
        return NULL;
    }

    for (sa_U64_t i = 0; i < verticePosAmount; i++) {
        float r =
            (rand() % 10001) / 10000.0f; // Generates a float between 0 and 1
        float g = (rand() % 10001) / 10000.0f;
        float b = (rand() % 10001) / 10000.0f;
        mesh->vertices[i].pos = verticesPos[i];
        mesh->vertices[i].texCoord = verticesTexcoord[i];
        mesh->vertices[i].color =
            (sa_Color_t){r, g, b, 1.0f}; // Default white color
    }

    mesh->indices = (sa_U32_t*)malloc(sizeof(sa_U32_t) * mesh->indicesAmount);
    if (!mesh->indices) {
        free(mesh->vertices);
        free(mesh);
        return NULL;
    }

    for (sa_U64_t i = 0; i < indiceAmount; i++) {
        mesh->indices[i] = indices[i].vertexIndex;
    }

    mesh->ibo = sc_GL_CreateIndexBuffer(mesh->indices, indiceAmount);

    free(verticesPos);
    free(verticesTexcoord);
    free(indices);

    return mesh;
}

struct sc_ModelMesh_c* sc_ModelMesh_Load(const char* path, sc_OBJ_ModelFileReadingFunction fileReader) {
    sa_Vec3_t* verticesPos = NULL;
    sa_U64_t verticesAmount = 0;
    sa_Vec2_t* verticesTexCoords = NULL;
    sa_U64_t verticesTexCoordsAmount = 0;
    struct sc_VertexIndice* indices = NULL;
    sa_U64_t indicesAmount = 0;

    if (!sc_OBJ_Parse(path, fileReader, &verticesPos, &verticesAmount,
                      &verticesTexCoords, &verticesTexCoordsAmount, &indices,
                      &indicesAmount)) {
        return NULL;
    }
    return sc_ModelMesh_Create(verticesPos, verticesAmount, verticesTexCoords,
                               verticesTexCoordsAmount, indices, indicesAmount);
}

void sc_ModelMesh_Delete(struct sc_ModelMesh_c* modelMesh) {
    if (!modelMesh)
        return;
    if (modelMesh->indices)
        free(modelMesh->indices);
    if (modelMesh->vertices)
        free(modelMesh->vertices);
    if (modelMesh->ibo)
        glDeleteBuffers(1, &modelMesh->ibo);
    free(modelMesh);
}

/* === Helper Implementation === */

void __sc_Renderer_RemoveGarbageNumbers(sc_Renderer_t* renderer) {
    renderer->renderBatch.renderCalls = NULL;
    renderer->renderBatch.renderCallCount = 0;
    renderer->renderBatch.capacity = 0;
    renderer->vao = 0;
    renderer->vbo = 0;
}

void __sc_Renderer_InitAll(sc_Renderer_t* renderer) {
    sc_Renderer_ResizeRenderBuffer(renderer, SACI_RENDER_BATCH_DEFAULT_CAPACITY);

    __sc_Renderer_InitGLVertexAttribContext(renderer);
    __sc_Renderer_InitShaderProgram(renderer);
#if defined(SACI_DEBUG_MODE) || defined(SACI_DEBUG_MODE_RENDERING)
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                        "Renderer initialized successfully");
#endif
}

sc_RenderCall __sc_RenderCall_Create(Arena* arena, struct sc_Vertice_c* vertices, sa_U64_t verticesAmount,
                                     sa_U64_t indicesAmount,
                                     int renderMode, sa_Texture_ID texID, sa_U32_t ibo,
                                     sa_Mat4_t modelMatrix) {
    sa_ASSERT(arena);
    sa_ASSERT(vertices);

    struct sc_Vertice_c* arenaVertices = ArenaAlloc(arena, sizeof(struct sc_Vertice_c) * verticesAmount);
    sa_ASSERT(arenaVertices);

    memcpy(arenaVertices, vertices, sizeof(struct sc_Vertice_c) * verticesAmount);

    sc_RenderCall renderCall = {
        .vertices = arenaVertices,
        .verticeAmount = verticesAmount,

        .indiceAmount = indicesAmount,

        .renderMode = renderMode,
        .textureID = texID,
        .ibo = ibo,

        .modelMatrix = modelMatrix,
    };
    return renderCall;
}

void __sc_RenderBatch_Push(sc_RenderBatch* renderBatch,
                           sc_RenderCall renderCall) {
    if (renderBatch->capacity <= renderBatch->renderCallCount) {
        // todo add option to resize after size reached
        sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                            "RenderBatch RenderCount exceeded Capacity ");
        return;
    }
    renderBatch->renderCalls[renderBatch->renderCallCount] = renderCall;
    renderBatch->renderCallCount++;
}

void __sc_RenderBatch_Empty(sc_RenderBatch* renderBatch) {
    renderBatch->renderCallCount = 0;
    renderBatch->renderCalls = NULL;
}

void __sc_RenderBatch_Free(sc_RenderBatch* renderBatch) {
    free(renderBatch->renderCalls);
    free(renderBatch);
}

void __sc_Renderer_InitGLVertexAttribContext(sc_Renderer_t* renderer) {
    sc_GL_CreateVertexArray(1, &renderer->vao);
    sc_GL_BindVertexArray(renderer->vao);

    renderer->vbo = sc_GL_CreateVertexBuffer(renderer->renderBatch.capacity * sizeof(struct sc_Vertice_c), NULL, GL_DYNAMIC_DRAW);
    sc_GL_BindVertexBuffer(renderer->vbo);

    sc_GL_SetVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct sc_Vertice_c),
                                 (void*)offsetof(struct sc_Vertice_c, pos));
    sc_GL_EnableVertexAttribArray(0);
    sc_GL_SetVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct sc_Vertice_c),
                                 (void*)offsetof(struct sc_Vertice_c, color));
    sc_GL_EnableVertexAttribArray(1);
    sc_GL_SetVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct sc_Vertice_c),
                                 (void*)offsetof(struct sc_Vertice_c, texCoord));
    sc_GL_EnableVertexAttribArray(2);
}

void __sc_Renderer_InitShaderProgram(sc_Renderer_t* renderer) {
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

    sa_U32_t vShader = sc_Shader_CompileShaderV(vShaderSource);
    sa_U32_t fShader = sc_Shader_CompileShaderF(fShaderSource);
    assert(vShader != 0 && fShader != 0);
    renderer->shaderProgram = sc_Shader_GetShaderProgram(vShader, fShader);
    assert(renderer->shaderProgram);
}

void __sc_Renderer_SetUniform(sc_Renderer_t* renderer, const struct sc_Camera_c* camera,
                              sa_Mat4_t modelMatrix, bool useTexture) {
    sa_Mat4_t view = {0};
    sa_Mat4_t projection = {0};

    int viewLoc = glGetUniformLocation(renderer->shaderProgram, "uViewMatrix");
    int projLoc = glGetUniformLocation(renderer->shaderProgram, "uProjectionMatrix");
    int useCamLoc = glGetUniformLocation(renderer->shaderProgram, "uUseCam");
    int uModelMatrixLoc = glGetUniformLocation(renderer->shaderProgram, "uModelMatrix");
    int uTextureLoc = glGetUniformLocation(renderer->shaderProgram, "uTexture");
    int uUseTextureLoc = glGetUniformLocation(renderer->shaderProgram, "uUseTexture");

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

    view = sa_Mat4_Look_At(camera->position, camera->target, camera->up);

    switch (sc_RenderConfig.projectionMode) {
    case sa_RENDER_ORTHOGRAPHIC_PROJECTION: {
        projection = sa_Mat4_Ortho(-1, 1, -1, 1, camera->near, camera->far);
        break;
    }
    case sa_RENDER_PERSPECTIVE_PROJECTION: {
        projection = sa_Mat4_Perspective(camera->fov, camera->aspectRatio,
                                         camera->near, camera->far);
        break;
    }
    case sa_RENDER_CUSTOM_PROJECTION: {
        if (sc_RenderConfig.customProjectionFunction == NULL) {
            sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_RENDERER,
                                "Custom projection mode function not set");
            return;
        }
        projection = sc_RenderConfig.customProjectionFunction(*camera);
        break;
    }
    }
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view.m[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection.m[0][0]);
}
