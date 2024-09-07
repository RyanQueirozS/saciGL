#include "core.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#include "scgl.h"

//----------------------------------------------------------------------------//

saci_Color saciColorFromHex(uint32_t hexValue) {
    saci_Color color;
    color.a = (hexValue >> 24) & 0xFF;
    color.r = (hexValue >> 16) & 0xFF;
    color.g = (hexValue >> 8) & 0xFF;
    color.b = hexValue & 0xFF;
    return color;
}

saci_Color saciColorFrom8Bit(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    saci_Color color;
    color.r = r;
    color.b = b;
    color.g = g;
    color.a = a;
    return color;
}

//----------------------------------------------------------------------------//

saciWindow* saciCreateWindow(const saciWindowSpecs windowSpecs) {
    assert(scglGLFWInit() != -1);
    printf("INFO: Initialized GLFW\n");

    saciWindow* window = scglCreateWindow(windowSpecs.width, windowSpecs.height,
                                          windowSpecs.title,
                                          windowSpecs.monitor, windowSpecs.windowShare);
    assert(window != NULL);
    printf("INFO: Created window\n");
    scglMakeWindowContext(window);
    printf("INFO: Made window context\n");

    assert(scglGLEWInit() != -1);
    printf("INFO: Initialized GLEW\n");

    return window;
}

void saciTerminate(void) {
    // TODO remove use glfw
    glfwTerminate();
}

//----------------------------------------------------------------------------//

void saciBeginComposition(void) {
}

void saciDrawComposition() {}

//----------------------------------------------------------------------------//

void saciClearCanvas(const saci_Color color) {
    // TODO remove direct use of glew
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

//----------------------------------------------------------------------------//

uint32_t saciLoadShaderFromPath(const char* path, uint32_t shaderType) {
    switch (shaderType) {
        case GL_VERTEX_SHADER: {
            return scglCompileShaderV(path);
        }
        case GL_FRAGMENT_SHADER: {
            return scglCompileShaderF(path);
        }
        case GL_GEOMETRY_SHADER: {
            return scglCompileShaderG(path);
        }
        default: {
            return 0;
        }
    }
    return 0;
}

uint32_t saciCreateProgram(uint32_t shaders[], size_t shaderAmmount) {
    return scglGetShaderProgram(shaders, shaderAmmount);
}

//----------------------------------------------------------------------------//
