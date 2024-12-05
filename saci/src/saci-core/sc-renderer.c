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

sc_RenderCall __sc_RenderCall_Create(Arena* arena, struct sc_Vertice_c* vertices, saci_u64 verticesAmount,
                                     saci_u64 indicesAmount, int renderMode, saci_TextureID texID,
                                     saci_u32 ibo, saci_Mat4 modelMatrix);

void __sc_RenderBatch_Push(sc_RenderBatch* renderBatch, sc_RenderCall renderCall);

void __sc_RenderBatch_Empty(sc_RenderBatch* renderBatch);

void __sc_RenderBatch_Free(sc_RenderBatch* renderBatch);

void __sc_Renderer_InitGLVertexAttribContext(sc_Renderer_t* renderer);

void __sc_Renderer_InitShaderProgram(sc_Renderer_t* renderer);

void __sc_Renderer_InitAll(sc_Renderer_t* renderer);

void __sc_Renderer_SetUniform(sc_Renderer_t* renderer, const struct sc_Camera_c* camera,
                              saci_Mat4 modelMatrix, bool useTexture);

/* === Local Definitions === */

#define SACI_RENDER_BATCH_DEFAULT_CAPACITY 0x1000000
#define SACI_DEFAULT_TEXTURE_BUFFER_SIZE 1 // TODO

struct sc_Vertice_c {
    saci_Vec3 pos;
    saci_Color color;
    saci_Vec2 texCoord;
};

typedef struct sc_RenderCall {
    struct sc_Vertice_c* vertices;
    saci_u64 verticeAmount;

    saci_u64 indiceAmount;

    int renderMode; // LINE TRIANGLE or QUAD
    saci_TextureID textureID;
    saci_u32 ibo;

    saci_Mat4 modelMatrix;
} sc_RenderCall;

typedef struct sc_RenderBatch {
    sc_RenderCall* renderCalls;
    saci_u32 capacity;
    saci_u32 renderCallCount;
} sc_RenderBatch;

// TODO doc
struct sc_Renderer {
    saci_u32 vao, vbo;
    saci_u32 shaderProgram;

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
    saci_u64 verticesAmount;

    saci_u32* indices;
    saci_u64 indicesAmount;
    saci_u32 ibo;
};

/* === Renderer Implementation === */

saci_Vec3 sc_Vertice_GetPos(const struct sc_Vertice_c* vertice) {
    return vertice->pos;
}

saci_Color sc_Vertice_GetColor(const struct sc_Vertice_c* vertice) {
    return vertice->color;
}

saci_Vec2 sc_Vertice_GetTexcoord(const struct sc_Vertice_c* vertice) {
    return vertice->texCoord;
}

struct sc_Vertice_c* sc_Vertice_CreateVertice(saci_Vec3 position, saci_Color color, saci_Vec2 texcood) {
    struct sc_Vertice_c* vertice = (struct sc_Vertice_c*)malloc(sizeof(struct sc_Vertice_c));
    vertice->pos = position;
    vertice->color = color;
    vertice->texCoord = texcood;
    return vertice;
}

void sc_Vertice_GetArrayInfo(struct sc_Vertice_c* vertexArray, saci_u64 vertexArraySize,
                             saci_Vec3** positions,
                             saci_Color** colors,
                             saci_Vec2** texcoords) {
    if (!vertexArray) {
        // LOG TODO
        return;
    }
    *positions = (saci_Vec3*)malloc(sizeof(saci_Vec3) * vertexArraySize);
    *colors = (saci_Color*)malloc(sizeof(saci_Color) * vertexArraySize);
    *texcoords = (saci_Vec2*)malloc(sizeof(saci_Vec2) * vertexArraySize);
    // TODO check errors

    for (saci_u64 i = 0; i < vertexArraySize; ++i) {
        struct sc_Vertice_c vertex = vertexArray[i];
        (*positions)[i] = vertex.pos;
        (*colors)[i] = vertex.color;
        (*texcoords)[i] = vertex.texCoord;
    }
}

struct sc_Vertice_c* sc_Vertice_CreateVerticesArray(saci_Vec3* positions,
                                                    saci_Color* colors,
                                                    saci_Vec2* texcoords,
                                                    saci_u64 amount) {
    struct sc_Vertice_c* vertices = (struct sc_Vertice_c*)malloc(sizeof(struct sc_Vertice_c) * amount);
    if (!vertices) {
        sa_LOG_PRINT_m(sa_LOG_LEVEL_ERROR, sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                       "Could not allocate new vertices");
        return NULL;
    }
    for (saci_u64 i = 0; i < amount; ++i) {
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

saci_u64 sc_ModelMesh_GetVerticesAmount(const struct sc_ModelMesh_c* modelMesh) {
    return modelMesh->verticesAmount;
}

saci_u32* sc_ModelMesh_GetIndices(const struct sc_ModelMesh_c* modelMesh) {
    return modelMesh->indices;
}

saci_u64 sc_ModelMesh_GetIndicesAmount(const struct sc_ModelMesh_c* modelMesh) {
    return modelMesh->indicesAmount;
}

sc_Renderer_t* sc_Renderer_CreateEmpty() {
    sc_Renderer_t* renderer = (sc_Renderer_t*)malloc(sizeof(sc_Renderer_t));
    if (!renderer) {
        sa_LOG_PRINT_m(sa_LOG_LEVEL_ERROR, sa_LOG_CONTEXT_RENDERER,
                       "Renderer could not be initialized");
        return NULL;
    }
    __sc_Renderer_RemoveGarbageNumbers(renderer);
    return renderer;
}

sc_Renderer_t* sc_Renderer_CreateDefault() {
    sc_Renderer_t* renderer = sc_Renderer_CreateEmpty();
    if (!renderer) {
        sa_LOG_PRINT_m(sa_LOG_LEVEL_ERROR, sa_LOG_CONTEXT_RENDERER,
                       "Renderer could not be initialized");
        return NULL;
    }
    sc_Renderer_InitMemoryContext(renderer, SACI_RENDER_BATCH_DEFAULT_CAPACITY);

    __sc_Renderer_InitAll(renderer);
    sa_LOG_PRINT_m(sa_LOG_LEVEL_INFO, sa_LOG_CONTEXT_RENDERER,
                   "Renderer created successfully");
    return renderer;
}

void sc_Renderer_InitMemoryContext(sc_Renderer_t* renderer, saci_u64 size) {
    ArenaInit(&renderer->memoryContext, size);
}

void sc_Renderer_ResizeRenderBuffer(sc_Renderer_t* renderer, saci_u64 newSize) {
    sc_RenderBatch* renderBatch = &renderer->renderBatch;
    if (newSize <= 0 || newSize <= renderBatch->renderCallCount) {
        sa_LOG_PRINT_m(sa_LOG_LEVEL_ERROR, sa_LOG_CONTEXT_RENDERER,
                       "RenderBatch new size is not valid");
        return;
    }
    sc_RenderCall* newRenderCalls = (sc_RenderCall*)malloc(newSize * sizeof(sc_RenderCall));
    if (!newRenderCalls) {
        sa_LOG_PRINT_m(sa_LOG_LEVEL_ERROR, sa_LOG_CONTEXT_RENDERER,
                       "RenderBatch Couldn't create new render call");
        return;
    }
    memcpy(newRenderCalls, renderBatch->renderCalls, renderBatch->renderCallCount * sizeof(sc_RenderCall));
    free(renderBatch->renderCalls);
    renderBatch->renderCalls = newRenderCalls;
    renderBatch->capacity = newSize;
    sa_LOG_PRINT_m(sa_LOG_LEVEL_INFO, sa_LOG_CONTEXT_RENDERER,
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

    sa_LOG_PRINT_m(sa_LOG_LEVEL_INFO, sa_LOG_CONTEXT_RENDERER,
                   "Renderer deleted successfully");
}

void sc_Renderer_SetNoFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    sc_RenderConfig.shouldFillShape = SACI_FALSE;
    sa_LOG_PRINT_m(sa_LOG_LEVEL_INFO, sa_LOG_CONTEXT_RENDERER,
                   "Renderer set no fill mode");
}

void sc_Renderer_SetFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    sc_RenderConfig.shouldFillShape = SACI_TRUE;
    sa_LOG_PRINT_m(sa_LOG_LEVEL_INFO, sa_LOG_CONTEXT_RENDERER,
                   "Renderer set fill mode");
}

void sc_Renderer_EnableZBuffer(void) {
    glEnable(GL_DEPTH_TEST);
    sa_LOG_PRINT_m(sa_LOG_LEVEL_INFO, sa_LOG_CONTEXT_RENDERER,
                   "Renderer enabled Z buffer");
}

void sc_Renderer_SetProjectionMode(enum sc_Renderer_Projection_Mode_e renderProjectionMode) {
    sc_RenderConfig.projectionMode = renderProjectionMode;
    switch (renderProjectionMode) {
    case sa_RENDER_ORTHOGRAPHIC_PROJECTION: {
        sa_LOG_PRINT_m(sa_LOG_LEVEL_INFO, sa_LOG_CONTEXT_RENDERER,
                       "Renderer set projection mode to ORTHOGRAPHIC");
        break;
    }
    case sa_RENDER_PERSPECTIVE_PROJECTION: {
        sa_LOG_PRINT_m(sa_LOG_LEVEL_INFO, sa_LOG_CONTEXT_RENDERER,
                       "Renderer set projection mode to PERSPECTIVE");
        break;
    }
    case sa_RENDER_CUSTOM_PROJECTION: {
        sa_LOG_PRINT_m(sa_LOG_LEVEL_INFO, sa_LOG_CONTEXT_RENDERER,
                       "Renderer set projection mode to CUSTOM");
        break;
    }
    }
}

void sc_Renderer_SetCustomProjectionModeFunction(
    sc_Renderer_Custom_Projection_Func renderCustomProjectionMode) {
    sc_RenderConfig.customProjectionFunction = renderCustomProjectionMode;
    sa_LOG_PRINT_m(sa_LOG_LEVEL_INFO, sa_LOG_CONTEXT_RENDERER,
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

    for (saci_u32 i = 0; i < renderer->renderBatch.renderCallCount; ++i) {
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
                              saci_u64 verticeAmount,
                              saci_u64 indiceAmount, saci_Mat4 modelMatrix,
                              saci_TextureID texID, saci_u32 ibo) {
    if (!vertices) {
        sa_LOG_PRINT_m(sa_LOG_LEVEL_INFO, sa_LOG_CONTEXT_RENDERER, "Invalid vertices");
        return;
    }
    sc_RenderCall renderCall = __sc_RenderCall_Create(&renderer->memoryContext,
                                                      vertices, verticeAmount,
                                                      indiceAmount, GL_TRIANGLES,
                                                      texID, ibo, modelMatrix);
    __sc_RenderBatch_Push(&renderer->renderBatch, renderCall);
}

void sc_Renderer_PushModelMesh(sc_Renderer_t* renderer, struct sc_ModelMesh_c* mesh,
                               saci_Mat4 modelMatrix, saci_TextureID texID) {
    if (!mesh) {
        exit(1);
    }
    sc_Renderer_PushVertices(renderer, mesh->vertices, mesh->verticesAmount,
                             mesh->indicesAmount, modelMatrix,
                             texID, mesh->ibo);
}

struct sc_ModelMesh_c* sc_ModelMesh_Create(saci_Vec3* verticesPos,
                                           saci_u64 verticePosAmount,
                                           saci_Vec2* verticesTexcoord,
                                           saci_u64 verticesTexcoordAmount,
                                           struct sc_VertexIndice* indices,
                                           saci_u64 indiceAmount) {
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

    for (saci_u64 i = 0; i < verticePosAmount; i++) {
        float r =
            (rand() % 10001) / 10000.0f; // Generates a float between 0 and 1
        float g = (rand() % 10001) / 10000.0f;
        float b = (rand() % 10001) / 10000.0f;
        mesh->vertices[i].pos = verticesPos[i];
        mesh->vertices[i].texCoord = verticesTexcoord[i];
        mesh->vertices[i].color =
            (saci_Color){r, g, b, 1.0f}; // Default white color
    }

    mesh->indices = (saci_u32*)malloc(sizeof(saci_u32) * mesh->indicesAmount);
    if (!mesh->indices) {
        free(mesh->vertices);
        free(mesh);
        return NULL;
    }

    for (saci_u64 i = 0; i < indiceAmount; i++) {
        mesh->indices[i] = indices[i].vertexIndex;
    }

    mesh->ibo = sc_GL_CreateIndexBuffer(mesh->indices, indiceAmount);

    free(verticesPos);
    free(verticesTexcoord);
    free(indices);

    return mesh;
}

struct sc_ModelMesh_c* sc_ModelMesh_Load(const char* path, sc_OBJ_ModelFileReadingFunction fileReader) {
    saci_Vec3* verticesPos = NULL;
    saci_u64 verticesAmount = 0;
    saci_Vec2* verticesTexCoords = NULL;
    saci_u64 verticesTexCoordsAmount = 0;
    struct sc_VertexIndice* indices = NULL;
    saci_u64 indicesAmount = 0;

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
    sa_LOG_PRINT_m(sa_LOG_LEVEL_DEBUG, sa_LOG_CONTEXT_RENDERER,
                   "Renderer initialized successfully");
#endif
}

sc_RenderCall __sc_RenderCall_Create(Arena* arena, struct sc_Vertice_c* vertices, saci_u64 verticesAmount,
                                     saci_u64 indicesAmount,
                                     int renderMode, saci_TextureID texID, saci_u32 ibo,
                                     saci_Mat4 modelMatrix) {
    SACI_ASSERT(arena);
    SACI_ASSERT(vertices);

    struct sc_Vertice_c* arenaVertices = ArenaAlloc(arena, sizeof(struct sc_Vertice_c) * verticesAmount);
    SACI_ASSERT(arenaVertices);

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
        sa_LOG_PRINT_m(sa_LOG_LEVEL_WARN, sa_LOG_CONTEXT_RENDERER,
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

    saci_u32 vShader = sc_Shader_CompileShaderV(vShaderSource);
    saci_u32 fShader = sc_Shader_CompileShaderF(fShaderSource);
    assert(vShader != 0 && fShader != 0);
    renderer->shaderProgram = sc_Shader_GetShaderProgram(vShader, fShader);
    assert(renderer->shaderProgram);
}

void __sc_Renderer_SetUniform(sc_Renderer_t* renderer, const struct sc_Camera_c* camera,
                              saci_Mat4 modelMatrix, bool useTexture) {
    saci_Mat4 view = {0};
    saci_Mat4 projection = {0};

    int viewLoc = glGetUniformLocation(renderer->shaderProgram, "uViewMatrix");
    int projLoc = glGetUniformLocation(renderer->shaderProgram, "uProjectionMatrix");
    int useCamLoc = glGetUniformLocation(renderer->shaderProgram, "uUseCam");
    int uModelMatrixLoc = glGetUniformLocation(renderer->shaderProgram, "uModelMatrix");
    int uTextureLoc = glGetUniformLocation(renderer->shaderProgram, "uTexture");
    int uUseTextureLoc = glGetUniformLocation(renderer->shaderProgram, "uUseTexture");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view.m[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection.m[0][0]);
    glUniformMatrix4fv(uModelMatrixLoc, 1, GL_FALSE, &modelMatrix.m[0][0]);
    glUniform1i(useCamLoc, SACI_TRUE);

    glUniform1i(uTextureLoc, 0);
    glUniform1i(uUseTextureLoc, SACI_FALSE);

    if (useTexture) {
        glUniform1i(uUseTextureLoc, SACI_TRUE);
    }

    if (camera == NULL) {
        glUniform1i(useCamLoc, SACI_FALSE);
        return;
    }

    view = saci_LookAtMat4(camera->position, camera->target, camera->up);

    switch (sc_RenderConfig.projectionMode) {
    case sa_RENDER_ORTHOGRAPHIC_PROJECTION: {
        projection = saci_OrthoMat4(-1, 1, -1, 1, camera->near, camera->far);
        break;
    }
    case sa_RENDER_PERSPECTIVE_PROJECTION: {
        projection = saci_PerspectiveMat4(camera->fov, camera->aspectRatio,
                                          camera->near, camera->far);
        break;
    }
    case sa_RENDER_CUSTOM_PROJECTION: {
        if (sc_RenderConfig.customProjectionFunction == NULL) {
            sa_LOG_PRINT_m(sa_LOG_LEVEL_ERROR, sa_LOG_CONTEXT_RENDERER,
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
