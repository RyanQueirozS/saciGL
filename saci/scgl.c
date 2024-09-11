#include "scgl.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "helpers.h"

// TODO user defined
#define MAX_TRIANGLES 1024
#define MAX_VERTICES MAX_TRIANGLES * 3

//----------------------------------------------------------------------------//
// Helper functions

u32 __scgl_compileShader(const char* shaderSource, u32 shaderType);

//----------------------------------------------------------------------------//

int scglGLFWInit(void) {
    // All of this might change, this could perhaps be setup by the user
    int success = glfwInit();
    if (!success) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return 0;
}

int scglGLEWInit(void) {
    if (glewInit() != GLEW_OK) {
        return -1;
    }
    return 0;
}

//----------------------------------------------------------------------------//

scglWindow* scglCreateWindow(int width, int height, const char* title,
                             scglMonitor* monitor, scglWindow* share) {
    return glfwCreateWindow(width, height, title, monitor, share);
}

void scglMakeWindowContext(scglWindow* window) { glfwMakeContextCurrent(window); }

//----------------------------------------------------------------------------//

u32 scglCompileShaderV(const char* source) {
    return __scgl_compileShader(source, GL_VERTEX_SHADER);
}

u32 scglCompileShaderF(const char* source) {
    return __scgl_compileShader(source, GL_FRAGMENT_SHADER);
}

u32 scglCompileShaderG(const char* source) {
    return __scgl_compileShader(source, GL_GEOMETRY_SHADER);
}

u32 scglGetShaderProgram(u32 vshader, u32 fshader) {
    u32 programID = glCreateProgram();
    glAttachShader(programID, vshader);
    glAttachShader(programID, fshader);
    glLinkProgram(programID);

    s32 success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char errMessage[2048];
        int sizeReturned = 0;
        glGetProgramInfoLog(programID, 2048, &sizeReturned, errMessage);
        printf("ERROR: Could not link shader\n%s", errMessage);
        return 0;
    }
    glDetachShader(programID, vshader);
    glDetachShader(programID, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return programID;
}

u32 scglGetShaderProgramg(u32 vshader, u32 fshader, u32 gshader) {
    u32 programID = glCreateProgram();
    glAttachShader(programID, vshader);
    glAttachShader(programID, fshader);
    glAttachShader(programID, gshader);
    glLinkProgram(programID);

    s32 success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char errMessage[2048];
        int sizeReturned = 0;
        glGetProgramInfoLog(programID, 2048, &sizeReturned, errMessage);
        printf("ERROR: Could not link shader\n%s", errMessage);
        return 0;
    }
    glDetachShader(programID, vshader);
    glDetachShader(programID, fshader);
    glDetachShader(programID, gshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    glDeleteShader(gshader);

    return programID;
}

//----------------------------------------------------------------------------//
// Render

void scglClearBackground(color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

typedef struct Vertice {
    vec2 pos;
    color color;
} Vertice;

struct scglRenderer {
    u32 vao, vbo;

    u32 shaderProgram;

    Vertice vertices[MAX_VERTICES];
    u32 vertexCount;
};

void __scgl_initRendererVertex(scglRenderer* renderer) {
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

void __scgl_initRendererShaderProgram(scglRenderer* renderer) {
    const char* vShaderSource =
        "#version 330 core\n"
        "in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\n\0";

    const char* fShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";

    u32 vShader = scglCompileShaderV(vShaderSource);
    u32 fShader = scglCompileShaderF(fShaderSource);
    assert(vShader != 0 && fShader != 0);
    renderer->shaderProgram = scglGetShaderProgram(vShader, fShader);
    assert(renderer->shaderProgram);
}

void __scgl_initRenderer(scglRenderer* renderer) {
    __scgl_initRendererVertex(renderer);
    __scgl_initRendererShaderProgram(renderer);
}

scglRenderer* scglCreateRenderer(void) {
    scglRenderer* renderer = (scglRenderer*)malloc(sizeof(scglRenderer));
    __scgl_initRenderer(renderer);
    return renderer;
}

void scglBeginComposition(scglRenderer* renderer) {
    printf("%d\n", renderer->vertexCount);
    renderer->vertexCount = 0;
}

void scglRenderComposition(scglRenderer* renderer) {
    glUseProgram(renderer->shaderProgram);
    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->vertexCount * 3 * sizeof(Vertice), renderer->vertices);

    glDrawArrays(GL_TRIANGLES, 0, renderer->vertexCount * 3);
}

void scglDeleteRenderer(scglRenderer* renderer) {
    glDeleteBuffers(1, &renderer->vbo);
    glDeleteVertexArrays(1, &renderer->vao);

    glDeleteProgram(renderer->shaderProgram);
}

//----------------------------------------------------------------------------//
// Draw
//----------------------------------------------------------------------------//

void __scgl_pushTriangleToRenderer(scglRenderer* renderer,
                                   const vec2 a, const vec2 b, const vec2 c,
                                   const color aColor, const color bColor, const color cColor) {
    // Each verticie is multiplied by 3, as this will be rendered as triangles(3 sides);
    renderer->vertices[renderer->vertexCount * 3 + 0].pos = a;
    renderer->vertices[renderer->vertexCount * 3 + 0].color = aColor;
    renderer->vertices[renderer->vertexCount * 3 + 1].pos = b;
    renderer->vertices[renderer->vertexCount * 3 + 1].color = bColor;
    renderer->vertices[renderer->vertexCount * 3 + 2].pos = c;
    renderer->vertices[renderer->vertexCount * 3 + 2].color = cColor;
    renderer->vertexCount++;
}

void scglRenderDrawRect(scglRenderer* renderer, const saciRect rect, color color) {
    vec2 a, b, c, d;
    a = Vec2(rect.x, rect.y);
    b = Vec2(rect.x + rect.width, rect.y);
    c = Vec2(rect.x + rect.width, rect.y + rect.height);
    d = Vec2(rect.x, rect.y + rect.height);
    __scgl_pushTriangleToRenderer(renderer, a, b, c, color, color, color);
    __scgl_pushTriangleToRenderer(renderer, a, c, d, color, color, color);
}

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

u32 __scgl_compileShader(const char* shaderSource, u32 shaderType) {
    u32 shaderID = glCreateShader(shaderType);

    glShaderSource(shaderID, 1, &shaderSource, NULL);
    glCompileShader(shaderID);

    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char errMessage[2048];
        int sizeReturned = 0;
        glGetShaderInfoLog(shaderID, 2048, &sizeReturned, &errMessage[0]);

        glDeleteShader(shaderID);
        printf("ERROR: Shader compilation failed: %s\n", errMessage);
        return 0;
    }

    return shaderID;
}

//----------------------------------------------------------------------------//
