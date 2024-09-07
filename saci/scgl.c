#include "scgl.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//----------------------------------------------------------------------------//
// Helper functions

const char* __scgl_loadFile(const char* filePath);
uint32_t __scgl_compileShader(const char* shaderSource, uint32_t shaderType);

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
    scglWindow* window = glfwCreateWindow(width, height, title, monitor, share);
    return window;
}

void scglMakeWindowContext(scglWindow* window) { glfwMakeContextCurrent(window); }

//----------------------------------------------------------------------------//

uint32_t scglCompileShaderV(const char* filePath) {
    const char* source = __scgl_loadFile(filePath);
    if (!source) {
        return 0;
    }

    return __scgl_compileShader(source, GL_VERTEX_SHADER);
}

uint32_t scglCompileShaderF(const char* filePath) {
    const char* source = __scgl_loadFile(filePath);
    if (!source) {
        return 0;
    }

    return __scgl_compileShader(source, GL_FRAGMENT_SHADER);
}

uint32_t scglCompileShaderG(const char* filePath) {
    const char* source = __scgl_loadFile(filePath);
    if (!source) {
        return 0;
    }

    return __scgl_compileShader(source, GL_GEOMETRY_SHADER);
}

uint32_t scglGetShaderProgram(const uint32_t shaders[], size_t shaderCount) {
    uint32_t programID = glCreateProgram();
    for (size_t i = 0; i < shaderCount; ++i) {
        glAttachShader(programID, shaders[i]);
    }
    glLinkProgram(programID);

    int32_t success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char errMessage[2048];
        int sizeReturned = 0;
        glGetProgramInfoLog(programID, 2048, &sizeReturned, errMessage);
        printf("ERROR: Could not link shader\n%s", errMessage);
        return 0;
    }

    return programID;
}

//----------------------------------------------------------------------------//
// Render

struct scglRenderer {
    // hold all of the renderElement's information/id
};

scglRenderer* scglCreateRenderer() {
    return (scglRenderer*)malloc(sizeof(scglRenderer));
}

//----------------------------------------------------------------------------//
// Helper functions

const char* __scgl_loadFile(const char* filePath) {
    FILE* shaderFile = fopen(filePath, "r");
    if (!shaderFile) {
        printf("INFO: Could not open shader file\n");
        return NULL;
    }
    fseek(shaderFile, 0L, SEEK_END);
    long fileSize = ftell(shaderFile);
    rewind(shaderFile);

    char* buffer = calloc(1, fileSize + 1);
    if (!buffer) {
        fclose(shaderFile);
        printf("INFO: Could not open shader file\n");
        return NULL;
    }

    if (fread(buffer, fileSize, 1, shaderFile) != 1) {
        fclose(shaderFile);
        printf("INFO: Could not open shader file\n");
        return NULL;
    }

    fclose(shaderFile);

    return buffer;
}

uint32_t __scgl_compileShader(const char* shaderSource, uint32_t shaderType) {
    uint32_t shaderID = glCreateShader(shaderType);

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
