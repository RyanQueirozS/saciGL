#include "saci-core/sc-rendering.h"
#include <glad/glad.h>

#include "saci-core.h"

#include "saci-core/sc-camera.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

// structs used in helper functions
typedef struct saci_Vertice saci_Vertice;
typedef struct saci_RenderCall saci_RenderCall;
typedef struct saci_RenderBatch saci_RenderBatch;

// This needs to be done to make each new renderer value = 0 or NULL. If not it
// will generate a garbage value and will lead to a crash
void __sc_initializeRenderValues(sc_Renderer* renderer);

// Renderer related
saci_RenderCall __sc_renderCall_create(saci_Vertice* vertices, int renderMode, saci_TextureID texID, saci_u64 verticesAmount);

void __sc_renderBatch_ResizeInternal(saci_RenderBatch* renderBatch, saci_u32 newSize);
void __sc_renderBatch_AddTo(saci_RenderBatch* renderBatch, saci_RenderCall renderCall);
void __sc_renderBatch_Empty(saci_RenderBatch* renderBatch);
void __sc_renderBatch_Free(saci_RenderBatch* renderBatch);

// OpenGL related
void __sc_resizeVBO(sc_Renderer* renderer, saci_u32 newCapacity);
void __sc_initRenderer_VBO_VAO(sc_Renderer* renderer);
void __sc_initRendererShaderProgram(sc_Renderer* renderer);
void __sc_initRenderer(sc_Renderer* renderer);

void __sc_setRenderUniform(sc_Renderer* renderer, const sc_Camera* camera);

//----------------------------------------------------------------------------//
// Base Definitions
//----------------------------------------------------------------------------//

// TODO rename
#define SACI_DEFAULT_MAX_TRIANGLES 1024
#define SACI_DEFAULT_VERTEX_BUFFER_SIZE SACI_DEFAULT_MAX_TRIANGLES * 3

#define SACI_DEFAULT_TEXTURE_BUFFER_SIZE 8

typedef struct saci_Vertice {
    saci_Vec3 pos;
    saci_Color color;
    saci_Vec2 texCoord;
} saci_Vertice;

typedef struct saci_RenderCall {
    saci_Vertice* vertices; // from opengl dot to opengl quad 0-7
    int renderMode;         // LINE TRIANGLE or QUAD
    saci_TextureID textureID;
} saci_RenderCall;

typedef struct saci_RenderBatch {
    saci_RenderCall* renderCalls;
    saci_u32 capacity;
    saci_u32 renderCallCount;
} saci_RenderBatch;

struct sc_Renderer {
    saci_u32 vao, vbo;

    saci_u32 shaderProgram;

    saci_RenderBatch renderBatch;
};

static struct sc_RenderConfig {
    sc_RendererProjectionMode projectionMode;
    sc_RendererCustomProjectionFunction customProjectionFunction;

    bool shouldFillShape;
} sc_renderConfig;

//----------------------------------------------------------------------------//
// Render Initialization/Deletion
//----------------------------------------------------------------------------//

sc_Renderer* sc_CreateRenderer(saci_Bool generateDefaults) {
    sc_Renderer* renderer = (sc_Renderer*)malloc(sizeof(sc_Renderer));
    if (!renderer) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_RENDERER, "Renderer could not be initialized");
        return NULL;
    }
    if (generateDefaults) {
        __sc_initRenderer(renderer);
    }
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer created successfully");
    return renderer;
}

void sc_DeleteRenderer(sc_Renderer* renderer) {
    glDeleteBuffers(1, &renderer->vbo);
    glDeleteVertexArrays(1, &renderer->vao);

    glDeleteProgram(renderer->shaderProgram);
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer deleted successfully");
}

//----------------------------------------------------------------------------//
// Renderer config
//----------------------------------------------------------------------------//

void sc_RenderSetNoFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    sc_renderConfig.shouldFillShape = SACI_FALSE;
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer set no fill mode");
}

void sc_RenderSetFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    sc_renderConfig.shouldFillShape = SACI_TRUE;
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer set fill mode");
}

void sc_RenderEnableZBuffer(void) {
    glEnable(GL_DEPTH_TEST);
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer enabled Z buffer");
}

void sc_RenderSetProjectionMode(sc_RendererProjectionMode renderProjectionMode) {
    sc_renderConfig.projectionMode = renderProjectionMode;
    switch (renderProjectionMode) {
        case SACI_RENDER_ORTHOGRAPHIC_PROJECTION: {
            SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer set projection mode to ORTHOGRAPHIC");
            break;
        }
        case SACI_RENDER_PERSPECTIVE_PROJECTION: {
            SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer set projection mode to PERSPECTIVE");
            break;
        }
        case SACI_RENDER_CUSTOM_PROJECTION: {
            SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer set projection mode to CUSTOM");
            break;
        }
    }
}

void sc_RenderSetCustomProjectionModeFunction(sc_RendererCustomProjectionFunction renderCustomProjectionMode) {
    sc_renderConfig.customProjectionFunction = renderCustomProjectionMode;
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "Renderer set custom projection mode function");
}

//----------------------------------------------------------------------------//
// Renderer Usage
//----------------------------------------------------------------------------//

void sc_RenderBegin(sc_Renderer* renderer) {
    renderer->renderBatch.renderCallCount = 0;
}

void sc_RenderEnd(sc_Renderer* renderer, const sc_Camera* camera) {
    glUseProgram(renderer->shaderProgram);

    __sc_setRenderUniform(renderer, camera);

    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

    for (saci_u32 i = 0; i < renderer->renderBatch.renderCallCount; ++i) {
        saci_RenderCall* call = &renderer->renderBatch.renderCalls[i];

        if (call->textureID != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, call->textureID);
        }

        saci_u32 vertexCount = 0;
        if (call->renderMode == GL_TRIANGLES) {
            vertexCount = 3; // Triangle needs 3 vertices
        } else if (call->renderMode == GL_QUADS) {
            vertexCount = 6; // A quad uses 6 vertices (2 triangles)
        } else if (call->renderMode == GL_LINES) {
            vertexCount = 2; // Line needs 2 vertices
        }

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(saci_Vertice) * vertexCount, call->vertices);

        if (call->renderMode == GL_TRIANGLES) {
            glDrawArrays(GL_TRIANGLES, 0, 3); // Draw 1 triangle
        } else if (call->renderMode == GL_QUADS) {
            glDrawArrays(GL_TRIANGLES, 0, 6); // Draw 2 triangles (6 vertices) for the quad
        } else if (call->renderMode == GL_LINES) {
            glDrawArrays(GL_LINES, 0, 2); // Draw 1 line (2 vertices)
        }

        if (call->textureID != 0) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    glBindVertexArray(0);
    glUseProgram(0);
}

void sc_RenderPushTriangleTexture(sc_Renderer* renderer,
                                  const saci_Vec3 a, const saci_Vec3 b, const saci_Vec3 c,
                                  const saci_Color aColor, const saci_Color bColor, const saci_Color cColor,
                                  const saci_Vec2 aUV, const saci_Vec2 bUV, const saci_Vec2 cUV,
                                  const saci_TextureID texID) {
    saci_Vertice vertices[] = {
        (saci_Vertice){a, aColor, aUV},
        (saci_Vertice){b, bColor, bUV},
        (saci_Vertice){c, cColor, cUV},
    };
    saci_RenderCall renderCall = __sc_renderCall_create(vertices, GL_TRIANGLES, texID, 3);
    __sc_renderBatch_AddTo(&renderer->renderBatch, renderCall);
}

void sc_RenderPushTriangle2D(sc_Renderer* renderer,
                             const saci_Vec2 a, const saci_Vec2 b, const saci_Vec2 c, float depth,
                             const saci_Color aColor, const saci_Color bColor, const saci_Color cColor) {
    saci_Vec3 a3 = {a.x, a.y, depth};
    saci_Vec3 b3 = {b.x, b.y, depth};
    saci_Vec3 c3 = {c.x, c.y, depth};

    saci_Vertice vertices[] = {
        (saci_Vertice){a3, aColor, {0, 0}},
        (saci_Vertice){b3, bColor, {0, 0}},
        (saci_Vertice){c3, cColor, {0, 0}},
    };
    saci_RenderCall renderCall = __sc_renderCall_create(vertices, GL_TRIANGLES, 0, 3);
    __sc_renderBatch_AddTo(&renderer->renderBatch, renderCall);
}

void sc_RenderPushTriangle3D(sc_Renderer* renderer,
                             const saci_Vec3 a, const saci_Vec3 b, const saci_Vec3 c,
                             const saci_Color aColor, const saci_Color bColor, const saci_Color cColor) {
    saci_Vertice vertices[] = {
        (saci_Vertice){a, aColor, {0, 0}},
        (saci_Vertice){b, bColor, {0, 0}},
        (saci_Vertice){c, cColor, {0, 0}},
    };
    saci_RenderCall renderCall = __sc_renderCall_create(vertices, GL_TRIANGLES, 0, 3);
    __sc_renderBatch_AddTo(&renderer->renderBatch, renderCall);
}

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

void __sc_initializeRenderValues(sc_Renderer* renderer) {
    renderer->renderBatch.renderCalls = NULL;
    renderer->renderBatch.renderCallCount = 0;
    renderer->renderBatch.capacity = 0;
}

saci_RenderCall __sc_renderCall_create(saci_Vertice* vertices, int renderMode, saci_TextureID texID, saci_u64 verticesAmount) {
    saci_RenderCall renderCall = {0};
    if (!vertices || verticesAmount == 0) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "RenderCall invalid vertices or size");
        return renderCall;
    }
    if (renderMode < 0 || renderMode > 7) { // TODO recreate opengl types
        SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "RenderCall invalid vertices or size");
        return renderCall;
    }
    renderCall.vertices = (saci_Vertice*)malloc(verticesAmount * sizeof(saci_Vertice));
    if (!renderCall.vertices) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "RenderCall couldn't allocate memory for vertices");
        return renderCall;
    }
    memcpy(renderCall.vertices, vertices, verticesAmount * sizeof(saci_Vertice));
    renderCall.renderMode = renderMode;
    renderCall.textureID = texID;

    return renderCall;
}

void __sc_renderBatch_ResizeInternal(saci_RenderBatch* renderBatch, saci_u32 newSize) {
    if (newSize <= 0 || newSize <= renderBatch->renderCallCount) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_RENDERER, "RenderBatch new size is not valid");
        return;
    }
    saci_RenderCall* newRenderCalls = (saci_RenderCall*)malloc(newSize * sizeof(saci_RenderCall));
    if (!newRenderCalls) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_RENDERER, "RenderBatch Couldn't create new render call");
        return;
    }
    memcpy(newRenderCalls, renderBatch->renderCalls, renderBatch->renderCallCount * sizeof(saci_RenderCall));
    free(renderBatch->renderCalls);
    renderBatch->renderCalls = newRenderCalls;
    renderBatch->capacity = newSize;
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER, "RenderBatch resized successfully");
}

void __sc_renderBatch_AddTo(saci_RenderBatch* renderBatch, saci_RenderCall renderCall) {
    if (renderBatch->capacity <= renderBatch->renderCallCount) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_WARN, SACI_LOG_CONTEXT_RENDERER, "RenderBatch RenderCount exceeded Capacity ");
        return;
    }
    renderBatch->renderCalls[renderBatch->renderCallCount] = renderCall;
    renderBatch->renderCallCount++;
}

void __sc_renderBatch_Empty(saci_RenderBatch* renderBatch) {
    renderBatch->renderCallCount = 0;
    renderBatch->renderCalls = NULL;
}

void __sc_renderBatch_Free(saci_RenderBatch* renderBatch) {
    free(renderBatch->renderCalls);
    free(renderBatch);
}

// OpenGL

void __sc_resizeVBO(sc_Renderer* renderer, saci_u32 newCapacity) {
    glBindVertexArray(renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, newCapacity * sizeof(saci_Vertice), NULL, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void __sc_initRenderer_VBO_VAO(sc_Renderer* renderer) {
    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(1, &renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, renderer->renderBatch.capacity * sizeof(saci_Vertice), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(saci_Vertice), (void*)offsetof(saci_Vertice, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(saci_Vertice), (void*)offsetof(saci_Vertice, color));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(saci_Vertice), (void*)offsetof(saci_Vertice, texCoord));
    glEnableVertexAttribArray(2);
}

void __sc_initRendererShaderProgram(sc_Renderer* renderer) {
    const char* vShaderSource =
        "#version 330 core\n"

        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec4 aColor;\n"
        "layout (location = 2) in vec2 aTexCoord;\n"

        "uniform mat4 uViewMatrix;\n"
        "uniform mat4 uProjectionMatrix;\n"
        "uniform bool uUseCam;\n"

        "out vec4 vColor;\n"
        "out vec2 vTexCoord;\n"

        "void main()\n"
        "{\n"
        "   if(uUseCam){\n"
        "       gl_Position = uProjectionMatrix * uViewMatrix * vec4(aPos, 1.0);\n"
        "   }else {\n"
        "       gl_Position = vec4(aPos, 1.0);\n"
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

    saci_u32 vShader = sc_CompileShaderV(vShaderSource);
    saci_u32 fShader = sc_CompileShaderF(fShaderSource);
    assert(vShader != 0 && fShader != 0);
    renderer->shaderProgram = sc_GetShaderProgram(vShader, fShader);
    assert(renderer->shaderProgram);
}

void __sc_initRenderer(sc_Renderer* renderer) {
    // Initializes to remove garbage numbers
    __sc_initializeRenderValues(renderer);

    { // Initializes the vertice and texture buffers with default sizes
        __sc_renderBatch_ResizeInternal(&renderer->renderBatch, SACI_DEFAULT_VERTEX_BUFFER_SIZE);
        assert(renderer->renderBatch.renderCalls);
    }

    // Initializes OpenGL shaders and objects
    __sc_initRenderer_VBO_VAO(renderer);
    __sc_initRendererShaderProgram(renderer);
#if defined(SACI_DEBUG_MODE) || defined(SACI_DEBUG_MODE_RENDERING)
    SACI_LOG_PRINT(SACI_LOG_LEVEL_DEBUG, SACI_LOG_CONTEXT_RENDERER, "Renderer initialized successfully");
#endif
}

void __sc_setRenderUniform(sc_Renderer* renderer, const sc_Camera* camera) {
    saci_Mat4 view = {0};
    saci_Mat4 projection = {0};

    if (camera == NULL) {
        int useCamLoc = glGetUniformLocation(renderer->shaderProgram, "uUseCam");
        glUniform1i(useCamLoc, SACI_FALSE);
        return;
    }

    view = saci_LookAtMat4(camera->position, camera->target, camera->up);

    switch (sc_renderConfig.projectionMode) {
        case SACI_RENDER_ORTHOGRAPHIC_PROJECTION: {
            projection = saci_OrthoMat4(-1, 1, -1, 1, camera->near, camera->far);
            break;
        }
        case SACI_RENDER_PERSPECTIVE_PROJECTION: {
            projection = saci_PerspectiveMat4(camera->fov, camera->aspectRatio, camera->near, camera->far);
            break;
        }
        case SACI_RENDER_CUSTOM_PROJECTION: {
            if (sc_renderConfig.customProjectionFunction == NULL) {
                return; // TODO: Handle error if no custom projection function is provided
            }
            projection = sc_renderConfig.customProjectionFunction(*camera);
            break;
        }
    }

    int viewLoc = glGetUniformLocation(renderer->shaderProgram, "uViewMatrix");
    int projLoc = glGetUniformLocation(renderer->shaderProgram, "uProjectionMatrix");
    int useCamLoc = glGetUniformLocation(renderer->shaderProgram, "uUseCam");
    int uTextureLoc = glGetUniformLocation(renderer->shaderProgram, "uTexture");
    int uUseTextureLoc = glGetUniformLocation(renderer->shaderProgram, "uUseTexture");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view.m[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection.m[0][0]);
    glUniform1i(useCamLoc, SACI_TRUE);

    glUniform1i(uTextureLoc, 0);
    glUniform1i(uUseTextureLoc, SACI_TRUE);
}
