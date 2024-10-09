#include <glad/glad.h>

#include "saci-core.h"

#include "saci-core/sc-camera.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-math.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

void __sc_initRendererVertex(sc_Renderer* renderer);
void __sc_initRendererShaderProgram(sc_Renderer* renderer);
void __sc_initRenderer(sc_Renderer* renderer);

void __sc_setRenderUniform(sc_Renderer* renderer, const sc_Camera* camera);

//----------------------------------------------------------------------------//
// Base Definitions
//----------------------------------------------------------------------------//

#define MAX_TRIANGLES 1024
#define MAX_VERTICES MAX_TRIANGLES * 3

typedef struct Vertice {
    saci_Vec3 pos;
    saci_Color color;
} Vertice;

struct sc_Renderer {
    saci_u32 vao, vbo;

    saci_u32 shaderProgram;

    Vertice vertices[MAX_VERTICES];
    saci_u32 vertexCount;
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
    renderer->vertexCount = 0;
}

void sc_RenderEnd(sc_Renderer* renderer, const sc_Camera* camera) {
    glUseProgram(renderer->shaderProgram);

    __sc_setRenderUniform(renderer, camera);

    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->vertexCount * 3 * sizeof(Vertice), renderer->vertices);

    glDrawArrays(GL_TRIANGLES, 0, renderer->vertexCount * 3);
}

void sc_RenderPushTriangle2D(sc_Renderer* renderer,
                             const saci_Vec2 a, const saci_Vec2 b, const saci_Vec2 c, float depth,
                             const saci_Color aColor, const saci_Color bColor, const saci_Color cColor) {
    // Each verticie is multiplied by 3, as this will be rendered as triangles(3 sides);
    renderer->vertices[renderer->vertexCount * 3 + 0].pos = (saci_Vec3){a.x, a.y, depth};
    renderer->vertices[renderer->vertexCount * 3 + 0].color = aColor;
    renderer->vertices[renderer->vertexCount * 3 + 1].pos = (saci_Vec3){b.x, b.y, depth};
    renderer->vertices[renderer->vertexCount * 3 + 1].color = bColor;
    renderer->vertices[renderer->vertexCount * 3 + 2].pos = (saci_Vec3){c.x, c.y, depth};
    renderer->vertices[renderer->vertexCount * 3 + 2].color = cColor;
    renderer->vertexCount++;
}

void sc_RenderPushTriangle3D(sc_Renderer* renderer,
                             const saci_Vec3 a, const saci_Vec3 b, const saci_Vec3 c,
                             const saci_Color aColor, const saci_Color bColor, const saci_Color cColor) {
    // Each verticie is multiplied by 3, as this will be rendered as triangles(3 sides);
    renderer->vertices[renderer->vertexCount * 3 + 0].pos = a;
    renderer->vertices[renderer->vertexCount * 3 + 0].color = aColor;
    renderer->vertices[renderer->vertexCount * 3 + 1].pos = b;
    renderer->vertices[renderer->vertexCount * 3 + 1].color = bColor;
    renderer->vertices[renderer->vertexCount * 3 + 2].pos = c;
    renderer->vertices[renderer->vertexCount * 3 + 2].color = cColor;
    renderer->vertexCount++;
}

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

void __sc_initRendererVertex(sc_Renderer* renderer) {
    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(1, &renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(Vertice), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, color));
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
    __sc_initRendererVertex(renderer);
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
