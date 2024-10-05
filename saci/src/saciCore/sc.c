#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "saci-core.h"
//
// TODO user defined
#define MAX_TRIANGLES 1024
#define MAX_VERTICES MAX_TRIANGLES * 3

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

saci_u32 __saciGL_compileShader(const char* shaderSource, saci_u32 shaderType);

void __saciGL_initRendererVertex(saciGL_Renderer* renderer);
void __saciGL_initRendererShaderProgram(saciGL_Renderer* renderer);
void __saciGL_initRenderer(saciGL_Renderer* renderer);

//----------------------------------------------------------------------------//
// Windowing
//----------------------------------------------------------------------------//

bool saciGL_GLFWInit(void) {
    int success = glfwInit();
    if (!success) return false;
    // TODO make user defined version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return true;
}

bool saciGL_GLEWInit(void) {
    if (glewInit() != GLEW_OK) {
        return false;
    }
    return true;
}

saciGL_Window* saciGL_CreateWindow(int width, int height, const char* title,
                                   saciGL_Monitor* monitor, saciGL_Window* share) {
    return glfwCreateWindow(width, height, title, monitor, share);
}
void saciGL_MakeWindowContext(saciGL_Window* window) { glfwMakeContextCurrent(window); }

void saciGL_SetWindowPosHandler(saciGL_Window* window, saciGL__WindowPosHandler windowPosHandler) {
    glfwSetWindowPosCallback(window, windowPosHandler);
}
void saciGL_SetWindowSizeHandler(saciGL_Window* window, saciGL__WindowSizeHandler windowSizeHandler) {
    glfwSetWindowSizeCallback(window, windowSizeHandler);
}

void saciGL_Terminate(void) { glfwTerminate(); }

void saciGL_ClearWindow(saci_Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void saciGL_SwapWindowBuffer(saciGL_Window* window) {
    glfwSwapBuffers(window);
}

//----------------------------------------------------------------------------//
// Shadering
//----------------------------------------------------------------------------//

saci_u32 saciGL_CompileShaderV(const char* source) {
    return __saciGL_compileShader(source, GL_VERTEX_SHADER);
}

saci_u32 saciGL_CompileShaderF(const char* source) {
    return __saciGL_compileShader(source, GL_FRAGMENT_SHADER);
}

saci_u32 saciGL_CompileShaderG(const char* source) {
    return __saciGL_compileShader(source, GL_GEOMETRY_SHADER);
}

saci_u32 saciGL_GetShaderProgram(saci_u32 vshader, saci_u32 fshader) {
    saci_u32 programID = glCreateProgram();
    glAttachShader(programID, vshader);
    glAttachShader(programID, fshader);
    glLinkProgram(programID);

    saci_s32 success = GL_FALSE;
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

saci_u32 saciGL_GetShaderProgramg(saci_u32 vshader, saci_u32 fshader, saci_u32 gshader) {
    saci_u32 programID = glCreateProgram();
    glAttachShader(programID, vshader);
    glAttachShader(programID, fshader);
    glAttachShader(programID, gshader);
    glLinkProgram(programID);

    saci_s32 success = GL_FALSE;
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
    saci_Vec2 pos;
    saci_Color color;
} Vertice;

struct saciGL_Renderer {
    saci_u32 vao, vbo;

    saci_u32 shaderProgram;

    Vertice vertices[MAX_VERTICES];
    saci_u32 vertexCount;
};

saciGL_Renderer* saciGL_CreateRenderer(void) {
    saciGL_Renderer* renderer = (saciGL_Renderer*)malloc(sizeof(saciGL_Renderer));
    assert(renderer);
    __saciGL_initRenderer(renderer);
    return renderer;
}

void saciGL_DeleteRenderer(saciGL_Renderer* renderer) {
    glDeleteBuffers(1, &renderer->vbo);
    glDeleteVertexArrays(1, &renderer->vao);

    glDeleteProgram(renderer->shaderProgram);
}

void saciGL_RenderBegin(saciGL_Renderer* renderer) {
    renderer->vertexCount = 0;
}

void saciGL_RenderEnd(saciGL_Renderer* renderer) {
    glUseProgram(renderer->shaderProgram);
    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->vertexCount * 3 * sizeof(Vertice), renderer->vertices);

    glDrawArrays(GL_TRIANGLES, 0, renderer->vertexCount * 3);
}

void saciGL_RenderSetNoFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void saciGL_RenderSetFillMode(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void saciGL_RenderPushTriangle(saciGL_Renderer* renderer,
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
// Event
//----------------------------------------------------------------------------//

void saciGL__PollEvents() {
    glfwPollEvents();
}
void saciGL__WaitForEvents() {
    glfwWaitEvents();
}
void saciGL__WaitForEventsTimeout(double timeout) {
    glfwWaitEventsTimeout(timeout);
}

void saciGL__PostEmptyEvent() {
    glfwPostEmptyEvent();
}

void saciGL_SetMousePosHandler(saciGL_Window* window, saciGL__MousePosHandlerFunction mousePosHandlerFunction) {
    glfwSetCursorPosCallback(window, mousePosHandlerFunction);
}

void saciGL_SetCursorHandler(saciGL_Window* window, void (*func)(saciGL_Window*, double, double)) {
    glfwSetCursorPosCallback(window, func);
}

bool saciGL_IsKeyPressed(saciGL_Window* window, saciGL_Keycode keycode) {
    int keyState = glfwGetKey(window, keycode);
    if (keyState == GLFW_PRESS) return true;
    return false;
}

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

saci_u32 __saciGL_compileShader(const char* shaderSource, saci_u32 shaderType) {
    saci_u32 shaderID = glCreateShader(shaderType);

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

void __saciGL_initRendererVertex(saciGL_Renderer* renderer) {
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

void __saciGL_initRendererShaderProgram(saciGL_Renderer* renderer) {
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

    saci_u32 vShader = saciGL_CompileShaderV(vShaderSource);
    saci_u32 fShader = saciGL_CompileShaderF(fShaderSource);
    assert(vShader != 0 && fShader != 0);
    renderer->shaderProgram = saciGL_GetShaderProgram(vShader, fShader);
    assert(renderer->shaderProgram);
}

void __saciGL_initRenderer(saciGL_Renderer* renderer) {
    __saciGL_initRendererVertex(renderer);
    __saciGL_initRendererShaderProgram(renderer);
}

//----------------------------------------------------------------------------//
