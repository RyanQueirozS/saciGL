#include <glad/glad.h>

#include "saci-core.h"

#include "saci-core/sc-camera.h"
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

// This needs to be done to make each new renderer value = 0 or NULL. If not it
// will generate a garbage value and will lead to a crash
void __sc_initializeRenderValues(sc_Renderer* renderer);

void __sc_resizeVerticeBuffer(sc_Renderer* renderer, saci_u32 newSize);

void __sc_resizeVBO(sc_Renderer* renderer, saci_u32 newCapacity);
void __sc_initRenderer_VBO_VAO(sc_Renderer* renderer);
void __sc_initRendererShaderProgram(sc_Renderer* renderer);
void __sc_initRenderer(sc_Renderer* renderer);

void __sc_setRenderUniform(sc_Renderer* renderer, const sc_Camera* camera);

//----------------------------------------------------------------------------//
// Base Definitions
//----------------------------------------------------------------------------//

#define SACI_DEFAULT_MAX_TRIANGLES 1024
#define SACI_DEFAULT_VERTEX_BUFFER_SIZE SACI_DEFAULT_MAX_TRIANGLES * 3

#define SACI_DEFAULT_TEXTURE_BUFFER_SIZE 8

typedef struct sac_Vertice {
    saci_Vec3 pos;
    saci_Color color;
} saci_Vertice;

typedef struct saci_VerticeBuffer {
    saci_Vertice* vertices;
    saci_u32 capacity;
    saci_u32 verticeCount;
} saci_VerticeBuffer;

struct sc_Renderer {
    saci_u32 vao, vbo;

    saci_u32 shaderProgram;

    saci_VerticeBuffer verticeBuffer;
};

static struct sc_RenderConfig {
    sc_RendererProjectionMode projectionMode;
    sc_RendererCustomProjectionFunction customProjectionFunction;

    bool isFillingShape;
} sc_renderConfig;

//----------------------------------------------------------------------------//
// Render Initialization/Deletion
//----------------------------------------------------------------------------//

sc_Renderer* sc_CreateRenderer(saci_Bool generateDefaults) {
    sc_Renderer* renderer = (sc_Renderer*)malloc(sizeof(sc_Renderer));
    assert(renderer);
    if (generateDefaults) {
        __sc_initRenderer(renderer);
    }
    return renderer;
}

void sc_DeleteRenderer(sc_Renderer* renderer) {
    glDeleteBuffers(1, &renderer->vbo);
    glDeleteVertexArrays(1, &renderer->vao);

    glDeleteProgram(renderer->shaderProgram);
}

//----------------------------------------------------------------------------//
// Renderer config
//----------------------------------------------------------------------------//

void sc_RenderSetNoFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    sc_renderConfig.isFillingShape = SACI_FALSE;
}

void sc_RenderSetFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    sc_renderConfig.isFillingShape = SACI_TRUE;
}

void sc_RenderEnableZBuffer(void) {
    glEnable(GL_DEPTH_TEST);
}

void sc_RenderSetProjectionMode(sc_RendererProjectionMode renderProjectionMode) {
    sc_renderConfig.projectionMode = renderProjectionMode;
}

void sc_RenderSetCustomProjectionMode(sc_RendererCustomProjectionFunction renderCustomProjectionMode) {
    sc_renderConfig.customProjectionFunction = renderCustomProjectionMode;
}

//----------------------------------------------------------------------------//
// Renderer Usage
//----------------------------------------------------------------------------//

void sc_RenderBegin(sc_Renderer* renderer) {
    renderer->verticeBuffer.verticeCount = 0;
}

void sc_RenderEnd(sc_Renderer* renderer, const sc_Camera* camera) {
    glUseProgram(renderer->shaderProgram);

    __sc_setRenderUniform(renderer, camera);

    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->verticeBuffer.verticeCount * 3 * sizeof(saci_Vertice), renderer->verticeBuffer.vertices);

    glDrawArrays(GL_TRIANGLES, 0, renderer->verticeBuffer.verticeCount * 3);
}

void sc_RenderPushTriangle2D(sc_Renderer* renderer,
                             const saci_Vec2 a, const saci_Vec2 b, const saci_Vec2 c, float depth,
                             const saci_Color aColor, const saci_Color bColor, const saci_Color cColor) {
    // Each verticie is multiplied by 3, as this will be rendered as triangles(3 sides);
    renderer->verticeBuffer.vertices[renderer->verticeBuffer.verticeCount * 3 + 0].pos = (saci_Vec3){a.x, a.y, depth};
    renderer->verticeBuffer.vertices[renderer->verticeBuffer.verticeCount * 3 + 0].color = aColor;
    renderer->verticeBuffer.vertices[renderer->verticeBuffer.verticeCount * 3 + 1].pos = (saci_Vec3){b.x, b.y, depth};
    renderer->verticeBuffer.vertices[renderer->verticeBuffer.verticeCount * 3 + 1].color = bColor;
    renderer->verticeBuffer.vertices[renderer->verticeBuffer.verticeCount * 3 + 2].pos = (saci_Vec3){c.x, c.y, depth};
    renderer->verticeBuffer.vertices[renderer->verticeBuffer.verticeCount * 3 + 2].color = cColor;
    renderer->verticeBuffer.verticeCount++;
}

void sc_RenderPushTriangle3D(sc_Renderer* renderer,
                             const saci_Vec3 a, const saci_Vec3 b, const saci_Vec3 c,
                             const saci_Color aColor, const saci_Color bColor, const saci_Color cColor) {
    // Each verticie is multiplied by 3, as this will be rendered as triangles(3 sides);
    renderer->verticeBuffer.vertices[renderer->verticeBuffer.verticeCount * 3 + 0].pos = a;
    renderer->verticeBuffer.vertices[renderer->verticeBuffer.verticeCount * 3 + 0].color = aColor;
    renderer->verticeBuffer.vertices[renderer->verticeBuffer.verticeCount * 3 + 1].pos = b;
    renderer->verticeBuffer.vertices[renderer->verticeBuffer.verticeCount * 3 + 1].color = bColor;
    renderer->verticeBuffer.vertices[renderer->verticeBuffer.verticeCount * 3 + 2].pos = c;
    renderer->verticeBuffer.vertices[renderer->verticeBuffer.verticeCount * 3 + 2].color = cColor;
    renderer->verticeBuffer.verticeCount++;
}

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

void __sc_initializeRenderValues(sc_Renderer* renderer) {
    renderer->verticeBuffer.vertices = NULL;
    renderer->verticeBuffer.verticeCount = 0;
    renderer->verticeBuffer.capacity = 0;
}

void __sc_resizeVerticeBuffer(sc_Renderer* renderer, saci_u32 newSize) {
    if (newSize == 0 || newSize < renderer->verticeBuffer.verticeCount) {
        printf("WARNING: Invalid size(%d) for renderer, either delete or use the renderer\n", newSize);
        return;
    }

    saci_Vertice* newTextures = (saci_Vertice*)malloc(newSize * sizeof(saci_Vertice));
    if (newTextures == NULL) {
        printf("ERROR: Couldn't allocate memory for renderer\n");
        return;
    }

    // Copy old data to the new buffer
    if (renderer->verticeBuffer.vertices != NULL) {
        saci_u32 copySize = renderer->verticeBuffer.verticeCount * sizeof(saci_Vertice);
        printf("%d\n", copySize);
        if (copySize > newSize) {
            copySize = newSize; // Only copy as much as newSize allows
        }
        memcpy(newTextures, renderer->verticeBuffer.vertices, copySize);
        free(renderer->verticeBuffer.vertices); // Free the old buffer
    }

    renderer->verticeBuffer.vertices = newTextures;
    renderer->verticeBuffer.capacity = newSize;
    renderer->verticeBuffer.verticeCount = (newSize / sizeof(saci_TextureID)); // Example calculation
    __sc_resizeVBO(renderer, newSize);
}

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
    glBufferData(GL_ARRAY_BUFFER, renderer->verticeBuffer.capacity * sizeof(saci_Vertice), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(saci_Vertice), (void*)offsetof(saci_Vertice, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(saci_Vertice), (void*)offsetof(saci_Vertice, color));
    glEnableVertexAttribArray(1);
}

void __sc_initRendererShaderProgram(sc_Renderer* renderer) {
    const char* vShaderSource =
        "#version 330 core\n"

        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec4 aColor;\n"

        "uniform mat4 uViewMatrix;\n"
        "uniform mat4 uProjectionMatrix;\n"
        "uniform bool uUseCam;\n"

        "out vec4 vColor;\n"

        "void main()\n"
        "{\n"
        "   if(uUseCam){\n"
        "       gl_Position = uProjectionMatrix * uViewMatrix * vec4(aPos, 1.0);\n"
        "   }else {\n"
        "       gl_Position = vec4(aPos, 1.0);\n"
        "   }\n"
        "    vColor = aColor;\n"
        "}\n\0";

    const char* fShaderSource =
        "#version 330 core\n"

        "out vec4 FragColor;\n"

        "in vec4 vColor;\n"

        "void main()\n"
        "{\n"
        "  FragColor = vColor;\n"
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
        __sc_resizeVerticeBuffer(renderer, SACI_DEFAULT_VERTEX_BUFFER_SIZE);
        assert(renderer->verticeBuffer.verticeCount);
    }

    // Initializes OpenGL shaders and objects
    __sc_initRenderer_VBO_VAO(renderer);
    __sc_initRendererShaderProgram(renderer);
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
            projection = saci_OrthoMat4(-1, 1, -1, 1,
                                        camera->near, camera->far);
            break;
        }
        case SACI_RENDER_PERSPECTIVE_PROJECTION: {
            projection = saci_PerspectiveMat4(camera->fov, camera->aspectRatio,
                                              camera->near, camera->far);
            break;
        }
        case SACI_RENDER_CUSTOM_PROJECTION: {
            if (sc_renderConfig.customProjectionFunction == NULL) {
                return; // TODO error handling
            }
            projection = sc_renderConfig.customProjectionFunction(*camera);
            break;
        }
    }

    int viewLoc = glGetUniformLocation(renderer->shaderProgram, "uViewMatrix");
    int projLoc = glGetUniformLocation(renderer->shaderProgram, "uProjectionMatrix");
    int useCamLoc = glGetUniformLocation(renderer->shaderProgram, "uUseCam");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view.m[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection.m[0][0]);
    glUniform1i(useCamLoc, SACI_TRUE);
}
