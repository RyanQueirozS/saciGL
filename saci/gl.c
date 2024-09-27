#include "gl.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "utils.h"

// TODO user defined
#define MAX_TRIANGLES 1024
#define MAX_VERTICES MAX_TRIANGLES * 3

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

u32 __scgl_compileShader(const char* shaderSource, u32 shaderType);

void __scgl_initRendererVertex(scglRenderer* renderer);
void __scgl_initRendererShaderProgram(scglRenderer* renderer);
void __scgl_initRenderer(scglRenderer* renderer);

//----------------------------------------------------------------------------//
// Windowing
//----------------------------------------------------------------------------//

bool scglGLFWInit(void) {
    int success = glfwInit();
    if (!success) return false;
    // TODO make user defined version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return true;
}

bool scglGLEWInit(void) {
    if (glewInit() != GLEW_OK) {
        return false;
    }
    return true;
}

scglWindow* scglCreateWindow(int width, int height, const char* title,
                             scglMonitor* monitor, scglWindow* share) {
    return glfwCreateWindow(width, height, title, monitor, share);
}
void scglMakeWindowContext(scglWindow* window) { glfwMakeContextCurrent(window); }

void scglSetWindowPosHandler(scglWindow* window, scgl_WindowPosHandler windowPosHandler) {
    glfwSetWindowPosCallback(window, windowPosHandler);
}
void scglSetWindowSizeHandler(scglWindow* window, scgl_WindowSizeHandler windowSizeHandler) {
    glfwSetWindowSizeCallback(window, windowSizeHandler);
}

void scglTerminate(void) { glfwTerminate(); }

//----------------------------------------------------------------------------//
// Shadering
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
//----------------------------------------------------------------------------//

typedef struct Vertice {
    Vec2 pos;
    Color color;
} Vertice;

struct scglRenderer {
    u32 vao, vbo;

    u32 shaderProgram;

    Vertice vertices[MAX_VERTICES];
    u32 vertexCount;
};

scglRenderer* scglCreateRenderer(void) {
    scglRenderer* renderer = (scglRenderer*)malloc(sizeof(scglRenderer));
    assert(renderer);
    __scgl_initRenderer(renderer);
    return renderer;
}

void scglDeleteRenderer(scglRenderer* renderer) {
    glDeleteBuffers(1, &renderer->vbo);
    glDeleteVertexArrays(1, &renderer->vao);

    glDeleteProgram(renderer->shaderProgram);
}

void scglRenderBegin(scglRenderer* renderer) {
    renderer->vertexCount = 0;
}

void scglRenderEnd(scglRenderer* renderer) {
    glUseProgram(renderer->shaderProgram);
    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->vertexCount * 3 * sizeof(Vertice), renderer->vertices);

    glDrawArrays(GL_TRIANGLES, 0, renderer->vertexCount * 3);
}

void scglRenderSetNoFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void scglRenderSetFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void scglRenderPushTriangle(scglRenderer* renderer,
                            const Vec2 a, const Vec2 b, const Vec2 c,
                            const Color aColor, const Color bColor, const Color cColor) {
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
// Draw
//----------------------------------------------------------------------------//

void scglClearBackground(Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void scglPresentDrawing(scglWindow* window) {
    glfwSwapBuffers(window);
}

//----------------------------------------------------------------------------//
// Event
//----------------------------------------------------------------------------//

void scgl_PollEvents() {
    glfwPollEvents();
}
void scgl_WaitForEvents() {
    glfwWaitEvents();
}
void scgl_WaitForEventsTimeout(double timeout) {
    glfwWaitEventsTimeout(timeout);
}

void scgl_PostEmptyEvent() {
    glfwPostEmptyEvent();
}

void scglSetMousePosHandler(scglWindow* window, scgl_MousePosHandlerFunction mousePosHandlerFunction) {
    glfwSetCursorPosCallback(window, mousePosHandlerFunction);
}

void scglSetCursorHandler(scglWindow* window, void (*func)(scglWindow*, double, double)) {
    glfwSetCursorPosCallback(window, func);
}

bool scgl_IsKeyPressed(scglWindow* window, scglKeycode keycode) {
    int keyState = glfwGetKey(window, keycode);
    if (keyState == GLFW_PRESS) return true;
    return false;
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
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec4 aColor;\n"
        "out vec4 vColor;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
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

//----------------------------------------------------------------------------//
