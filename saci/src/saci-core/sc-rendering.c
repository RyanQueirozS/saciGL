#include <GL/glew.h>

#include "saci-core.h"

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
void __sc_initRenderCamera(sc_Renderer* renderer);
void __sc_initRenderer(sc_Renderer* renderer);

//----------------------------------------------------------------------------//

#define MAX_TRIANGLES 1024
#define MAX_VERTICES MAX_TRIANGLES * 3

typedef struct Vertice {
    saci_Vec2 pos;
    saci_Color color;
} Vertice;

struct sc_Renderer {
    saci_u32 vao, vbo;

    saci_u32 shaderProgram;

    Vertice vertices[MAX_VERTICES];
    saci_u32 vertexCount;
};

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

void sc_RenderBegin(sc_Renderer* renderer) {
    renderer->vertexCount = 0;
}

void sc_RenderEnd(sc_Renderer* renderer) {
    glUseProgram(renderer->shaderProgram);
    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->vertexCount * 3 * sizeof(Vertice), renderer->vertices);

    glDrawArrays(GL_TRIANGLES, 0, renderer->vertexCount * 3);
}

void sc_RenderSetNoFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void sc_RenderSetFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void sc_RenderPushTriangle(sc_Renderer* renderer,
                           const saci_Vec2 a, const saci_Vec2 b, const saci_Vec2 c,
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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, pos));
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

        "out vec4 vColor;\n"

        "void main()\n"
        "{\n"
        "  gl_Position = uProjectionMatrix * uViewMatrix * vec4(aPos, 1.0);\n"
        "  vColor = aColor;\n"
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

void __sc_initRenderCamera(sc_Renderer* renderer) {
    glUseProgram(renderer->shaderProgram);
    saci_Vec3 cameraPos = {0.0f, 0.0f, 0.0f};
    saci_Vec3 cameraTarget = {0.0f, 0.0f, 0.0f};
    saci_Vec3 upVector = {0.0f, 1.0f, 0.0f};
    saci_Mat4 view = saci_LookAtMat4(cameraPos, cameraTarget, upVector);

    float fov = 45.0f;
    float aspect = 1600.0f / 900.0f;
    float near = 0.1f;
    float far = 100.0f;
    saci_Mat4 projection = saci_PerspectiveMat4(fov, aspect, near, far);

    // Pass the matrices to the shaders
    int viewLoc = glGetUniformLocation(renderer->shaderProgram, "uViewMatrix");
    int projLoc = glGetUniformLocation(renderer->shaderProgram, "uProjectionMatrix");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view.m[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection.m[0][0]);
}

void __sc_initRenderer(sc_Renderer* renderer) {
    __sc_initRendererVertex(renderer);
    __sc_initRendererShaderProgram(renderer);
    __sc_initRenderCamera(renderer);
}
