#include <glad/glad.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "saci-core/sc-gl.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-math.h"

/* === Structs for Helper Functions === */

typedef struct sc_RenderCall sc_RenderCall;
typedef struct sc_RenderBatch sc_RenderBatch;

/* === Helper Functions === */

/**
 * @brief Function to cleanup garbage numbers.
 *
 * @details
 * This needs to be used so it doesn't crash the application
 *
 * @param renderer The sc_Renderer to initialized
 */
void __sc_Renderer_initializeValues(sc_Renderer* renderer);

/**
 * @brief Creates a sc_RenderCall
 *
 * @param vertices A vertice array containing all of the vertices that will be pushed to
 * the RenderCall.
 * @param renderMode The mode these vertices will be rendered,
 * (SACI_RENDERCALL_MODE_TRIANGLE, SACI_RENDERCALL_MODE_LINE or
 * SACI_RENDERCALL_MODE_QUAD).
 * @param texID The ID of the OpenGL texture that will be used on the render call, can be
 * 0.
 * @param verticesAmount The amount of vertices in the vertices array
 */
sc_RenderCall __sc_RenderCall_create(sc_Vertice* vertices, int renderMode, saci_TextureID texID,
                                     saci_u64 verticesAmount);

/**
 * @brief Resizes a sc_RenderBatch
 *
 * @param renderBatch The sc_RenderBatch to be resized
 * @param newSize The newSize of the renderBatch
 */
void __sc_renderBatch_resize(sc_RenderBatch* renderBatch, saci_u32 newSize);

/**
 * @brief pushes a sc_RenderCall to a sc_RenderBatch
 *
 * @param renderBatch The sc_RenderBatch the renderCall will be added
 * @param renderCall The sc_RenderCall to be pushed into the renderBatch
 */
void __sc_renderBatch_push(sc_RenderBatch* renderBatch, sc_RenderCall renderCall);

/**
 * @brief empties a sc_RenderBatch
 *
 * @param renderBatch The sc_RenderBatch that will be emptied
 */
void __sc_renderBatch_empty(sc_RenderBatch* renderBatch);

/**
 * @brief frees a sc_RenderBatch
 *
 * @param renderBatch The sc_RenderBatch that will be freed from memory
 */
void __sc_renderBatch_free(sc_RenderBatch* renderBatch);

/* === OpenGL Related === */

/**
 * @brief initializes OpenGL vertex attribute context
 *
 * @param renderer The renderer that will be initialized
 */
void __sc_renderer_initGLVertexAttribContext(sc_Renderer* renderer);

/**
 * @brief initializes OpenGL shader context
 *
 * @param renderer The renderer that will have it's shader program initialized
 */
void __sc_renderer_initShaderProgram(sc_Renderer* renderer);

/**
 * @brief initializes all of the above mentioned functions
 *
 * @param renderer The renderer that be initialized
 */
void __sc_renderer_initAll(sc_Renderer* renderer);

/**
 * @brief sets uniforms up
 *
 * @param renderer The renderer that bases the shaderProgram
 * @param camera The camera that will or won't be used
 * @param modelMatrix The modelMatrix referenced in the DrawCall
 * @param setTexture The texture that will or won't be used
 */
void __sc_renderer_setUniform(sc_Renderer* renderer, const sc_Camera* camera, saci_Mat4 modelMatrix,
                              bool useTexture);

/* === Local Definitions === */

#define SACI_RENDER_BATCH_DEFAULT_CAPACITY 0x1000000
#define SACI_DEFAULT_TEXTURE_BUFFER_SIZE 1 // TODO

typedef struct sc_Vertice {
    saci_Vec3 pos;
    saci_Color color;
    saci_Vec2 texCoord;
} sc_Vertice;

typedef struct sc_RenderCall {
    sc_Vertice* vertices;
    saci_u64 verticeAmount;
    int renderMode; // LINE TRIANGLE or QUAD
    saci_TextureID textureID;

    saci_u32* indices;
    saci_u64 indiceAmount;
    saci_u32 ibo;

    saci_Mat4 modelMatrix;
} sc_RenderCall;

typedef struct sc_RenderBatch {
    sc_RenderCall* renderCalls;
    saci_u32 capacity;
    saci_u32 renderCallCount;
} sc_RenderBatch;

struct sc_Renderer {
    saci_u32 vao, vbo;

    saci_u32 shaderProgram;

    sc_RenderBatch renderBatch;
};

static struct sc_RenderConfig {
    sc_RendererProjectionMode projectionMode;
    sc_Renderer_CustomProjectionFunction customProjectionFunction;

    bool shouldFillShape;
} sc_RenderConfig;

typedef struct sc_ModelMesh {
    sc_Vertice* vertices;
    saci_u64 verticesAmount;

    saci_u32* indices;
    saci_u64 indicesAmount;
} sc_ModelMesh;

/* === Renderer Implementation === */

sc_Vertice* sc_Vertice_CreateVerticesArray(saci_Vec3* positions, saci_Color* colors,
                                           saci_Vec2* texcoords, saci_u64 amount) {
    sc_Vertice* vertices = (sc_Vertice*)malloc(sizeof(sc_Vertice) * amount);
    if (!vertices) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_MEMORY_ALLOCATION,
                       "Could not allocate new vertices");
        return NULL;
    }
    for (saci_u64 i = 0; i < amount; ++i) {
        if (positions) vertices[i].pos = positions[i];
        if (colors) vertices[i].color = colors[i];
        if (texcoords) vertices[i].texCoord = texcoords[i];
    }
    return vertices;
}

sc_Renderer* sc_Renderer_Create(saci_Bool generateDefaults) {
    sc_Renderer* renderer = (sc_Renderer*)malloc(sizeof(sc_Renderer));
    if (!renderer) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_RENDERER,
                       "Renderer could not be initialized");
        return NULL;
    }
    if (generateDefaults) {
        __sc_renderer_initAll(renderer);
    }
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer created successfully");
    return renderer;
}

void sc_Renderer_Delete(sc_Renderer* renderer) {
    glDeleteBuffers(1, &renderer->vbo);
    glDeleteVertexArrays(1, &renderer->vao);

    glDeleteProgram(renderer->shaderProgram);
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer deleted successfully");
}

void sc_Renderer_SetNoFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    sc_RenderConfig.shouldFillShape = SACI_FALSE;
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer set no fill mode");
}

void sc_Renderer_SetFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    sc_RenderConfig.shouldFillShape = SACI_TRUE;
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer set fill mode");
}

void sc_Renderer_EnableZBuffer(void) {
    glEnable(GL_DEPTH_TEST);
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer enabled Z buffer");
}

void sc_Renderer_SetProjectionMode(sc_RendererProjectionMode renderProjectionMode) {
    sc_RenderConfig.projectionMode = renderProjectionMode;
    switch (renderProjectionMode) {
        case SACI_RENDER_ORTHOGRAPHIC_PROJECTION: {
            SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER,
                           "Renderer set projection mode to ORTHOGRAPHIC");
            break;
        }
        case SACI_RENDER_PERSPECTIVE_PROJECTION: {
            SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER,
                           "Renderer set projection mode to PERSPECTIVE");
            break;
        }
        case SACI_RENDER_CUSTOM_PROJECTION: {
            SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER,
                           "Renderer set projection mode to CUSTOM");
            break;
        }
    }
}

void sc_Renderer_SetCustomProjectionModeFunction(
    sc_Renderer_CustomProjectionFunction renderCustomProjectionMode) {
    sc_RenderConfig.customProjectionFunction = renderCustomProjectionMode;
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER,
                   "Renderer set custom projection mode function");
}

void sc_Renderer_Begin(sc_Renderer* renderer) { renderer->renderBatch.renderCallCount = 0; }

void sc_Renderer_End(sc_Renderer* renderer, const sc_Camera* camera) {
    glUseProgram(renderer->shaderProgram);

    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

    for (saci_u32 i = 0; i < renderer->renderBatch.renderCallCount; ++i) {
        sc_RenderCall* call = &renderer->renderBatch.renderCalls[i];
        __sc_renderer_setUniform(renderer, camera, call->modelMatrix, call->textureID);

        if (call->textureID != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, call->textureID);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, call->ibo);

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sc_Vertice) * call->verticeAmount,
                        call->vertices);

        glDrawElements(call->renderMode, call->indiceAmount, GL_UNSIGNED_INT, 0);

        if (call->textureID != 0) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    glBindVertexArray(0);
    glUseProgram(0);
}

void sc_Renderer_PushVertices(sc_Renderer* renderer, sc_Vertice* vertices, saci_u64 verticeAmount,
                              saci_u32* indices, saci_u64 indiceAmount, saci_Mat4 modelMatrix,
                              saci_TextureID texID) {
    if (!vertices) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Invalid vertices");
        return;
    }
    sc_RenderCall renderCall = __sc_RenderCall_create(vertices, GL_TRIANGLES, texID, verticeAmount);
    renderCall.indices = indices;
    renderCall.indiceAmount = indiceAmount;
    renderCall.verticeAmount = verticeAmount;
    renderCall.modelMatrix = modelMatrix;
    renderCall.ibo = sc_GL_CreateIndexBuffer(renderCall.indices, renderCall.indiceAmount);
    __sc_renderBatch_push(&renderer->renderBatch, renderCall);
}

void sc_Renderer_PushModelMesh(sc_Renderer* renderer, sc_ModelMesh* mesh, saci_Mat4 modelMatrix,
                               saci_TextureID texID) {
    if (!mesh) {
        exit(1);
    }
    sc_Renderer_PushVertices(renderer, mesh->vertices, mesh->verticesAmount, mesh->indices,
                             mesh->indicesAmount, modelMatrix, texID);
}

sc_ModelMesh* sc_ModelMesh_Create(saci_Vec3* verticesPos, saci_u64 verticePosAmount,
                                  saci_Vec2* verticesTexcoord, saci_u64 verticesTexcoordAmount,
                                  struct sc_VertexIndice* indices, saci_u64 indiceAmount) {
    // TODO
    // if (verticePosAmount != verticesTexcoordAmount) {
    //     return NULL;
    // }

    sc_ModelMesh* mesh = (sc_ModelMesh*)malloc(sizeof(sc_ModelMesh));
    if (!mesh) {
        return NULL;
    }

    assert(verticePosAmount < SACI_RENDER_BATCH_DEFAULT_CAPACITY);

    mesh->verticesAmount = verticePosAmount;
    mesh->indicesAmount = indiceAmount;

    mesh->vertices = (sc_Vertice*)malloc(sizeof(sc_Vertice) * mesh->verticesAmount);
    if (!mesh->vertices) {
        free(mesh);
        return NULL;
    }

    for (saci_u64 i = 0; i < verticePosAmount; i++) {
        mesh->vertices[i].pos = verticesPos[i];
        mesh->vertices[i].texCoord = verticesTexcoord[i];
        mesh->vertices[i].color = (saci_Color){1.0f, 1.0f, 1.0f, 1.0f}; // Default white color
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

    return mesh;
}

sc_ModelMesh* sc_ModelMesh_Load(const char* path, sc_Model_FileReadingFunction fileReader) {
    char* buffer = 0;
    saci_u64 lenght = 0;
    fileReader(path, &buffer, &lenght);

    saci_Vec3* verticesPos = NULL;
    saci_u64 verticesAmount = 0;
    saci_Vec2* verticesTexCoords = NULL;
    saci_u64 verticesTexCoordsAmount = 0;
    struct sc_VertexIndice* indices = NULL;
    saci_u64 indicesAmount = 0;
    if (!sc_OBJ_Parse(buffer, lenght, &verticesPos, &verticesAmount, &verticesTexCoords,
                      &verticesTexCoordsAmount, &indices, &indicesAmount)) {
        return NULL;
    }
    return sc_ModelMesh_Create(verticesPos, verticesAmount, verticesTexCoords,
                               verticesTexCoordsAmount, indices, indicesAmount);
}

void sc_ModelMesh_Delete(sc_ModelMesh* modelMesh) {
    if (!modelMesh) return;
    free(modelMesh->indices);
    free(modelMesh->vertices);
    free(modelMesh);
}

/* === Helper Implementation === */

void __sc_Renderer_initializeValues(sc_Renderer* renderer) {
    renderer->renderBatch.renderCalls = NULL;
    renderer->renderBatch.renderCallCount = 0;
    renderer->renderBatch.capacity = 0;
}

void __sc_renderer_initAll(sc_Renderer* renderer) {
    // Initializes to remove garbage numbers
    __sc_Renderer_initializeValues(renderer);

    { // Initializes the vertice and texture buffers with default sizes
        __sc_renderBatch_resize(&renderer->renderBatch, SACI_RENDER_BATCH_DEFAULT_CAPACITY);
        assert(renderer->renderBatch.renderCalls);
    }

    // Initializes OpenGL shaders and objects
    __sc_renderer_initGLVertexAttribContext(renderer);
    __sc_renderer_initShaderProgram(renderer);
#if defined(SACI_DEBUG_MODE) || defined(SACI_DEBUG_MODE_RENDERING)
    SACI_LOG_PRINT(SACI_LOG_LEVEL_DEBUG, SACI_LOG_CONTEXT_RENDERER,
                   "Renderer initialized successfully");
#endif
}

sc_RenderCall __sc_RenderCall_create(sc_Vertice* vertices, int renderMode, saci_TextureID texID,
                                     saci_u64 verticesAmount) {
    sc_RenderCall renderCall = {0};
    if (!vertices || verticesAmount == 0) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER,
                       "RenderCall invalid vertices or size");
        return renderCall;
    }
    if (renderMode < 0 || renderMode > 7) { // TODO recreate opengl types
        SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER,
                       "RenderCall invalid vertices or size");
        return renderCall;
    }
    renderCall.vertices = (sc_Vertice*)malloc(verticesAmount * sizeof(sc_Vertice));
    if (!renderCall.vertices) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER,
                       "RenderCall couldn't allocate memory for vertices");
        return renderCall;
    }
    memcpy(renderCall.vertices, vertices, verticesAmount * sizeof(sc_Vertice));
    renderCall.renderMode = renderMode;
    renderCall.textureID = texID;

    return renderCall;
}

void __sc_renderBatch_resize(sc_RenderBatch* renderBatch, saci_u32 newSize) {
    if (newSize <= 0 || newSize <= renderBatch->renderCallCount) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_RENDERER,
                       "RenderBatch new size is not valid");
        return;
    }
    sc_RenderCall* newRenderCalls = (sc_RenderCall*)malloc(newSize * sizeof(sc_RenderCall));
    if (!newRenderCalls) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_RENDERER,
                       "RenderBatch Couldn't create new render call");
        return;
    }
    memcpy(newRenderCalls, renderBatch->renderCalls,
           renderBatch->renderCallCount * sizeof(sc_RenderCall));
    free(renderBatch->renderCalls);
    renderBatch->renderCalls = newRenderCalls;
    renderBatch->capacity = newSize;
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER,
                   "RenderBatch resized successfully");
}

void __sc_renderBatch_push(sc_RenderBatch* renderBatch, sc_RenderCall renderCall) {
    if (renderBatch->capacity <= renderBatch->renderCallCount) {
        // todo add option to resize after size reached
        SACI_LOG_PRINT(SACI_LOG_LEVEL_WARN, SACI_LOG_CONTEXT_RENDERER,
                       "RenderBatch RenderCount exceeded Capacity ");
        return;
    }
    renderBatch->renderCalls[renderBatch->renderCallCount] = renderCall;
    renderBatch->renderCallCount++;
}

void __sc_renderBatch_empty(sc_RenderBatch* renderBatch) {
    renderBatch->renderCallCount = 0;
    renderBatch->renderCalls = NULL;
}

void __sc_renderBatch_free(sc_RenderBatch* renderBatch) {
    free(renderBatch->renderCalls);
    free(renderBatch);
}

void __sc_renderer_initGLVertexAttribContext(sc_Renderer* renderer) {
    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(1, &renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, renderer->renderBatch.capacity * sizeof(sc_Vertice), NULL,
                 GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sc_Vertice),
                          (void*)offsetof(sc_Vertice, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(sc_Vertice),
                          (void*)offsetof(sc_Vertice, color));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sc_Vertice),
                          (void*)offsetof(sc_Vertice, texCoord));
    glEnableVertexAttribArray(2);
}

void __sc_renderer_initShaderProgram(sc_Renderer* renderer) {
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
        "       gl_Position = uProjectionMatrix * uViewMatrix * worldPosition;\n"
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

void __sc_renderer_setUniform(sc_Renderer* renderer, const sc_Camera* camera, saci_Mat4 modelMatrix,
                              bool useTexture) {
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
        case SACI_RENDER_ORTHOGRAPHIC_PROJECTION: {
            projection = saci_OrthoMat4(-1, 1, -1, 1, camera->near, camera->far);
            break;
        }
        case SACI_RENDER_PERSPECTIVE_PROJECTION: {
            projection =
                saci_PerspectiveMat4(camera->fov, camera->aspectRatio, camera->near, camera->far);
            break;
        }
        case SACI_RENDER_CUSTOM_PROJECTION: {
            if (sc_RenderConfig.customProjectionFunction == NULL) {
                SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_RENDERER,
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
