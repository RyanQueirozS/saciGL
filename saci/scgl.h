#ifndef __SACI_SGL_H__
#define __SACI_SGL_H__

#include <stdbool.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdint.h>

//----------------------------------------------------------------------------//

typedef GLFWmonitor scglMonitor;
typedef GLFWwindow scglWindow;

//----------------------------------------------------------------------------//
// Init

int scglGLFWInit(void);
int scglGLEWInit(void);

//----------------------------------------------------------------------------//
// Windowing

scglWindow* scglCreateWindow(int width, int height, const char* title,
                             scglMonitor* monitor, scglWindow* share);
void scglMakeWindowContext(scglWindow* window);

//----------------------------------------------------------------------------//
// Shadering

uint32_t scglCompileShaderV(const char* filePath);  // Remove from header
uint32_t scglCompileShaderF(const char* filePath);  // Remove from header
uint32_t scglCompileShaderG(const char* filePath);  // Remove from header

uint32_t scglGetShaderProgram(const uint32_t shaders[], size_t shaderCount);

//----------------------------------------------------------------------------//
// Render

typedef struct scglRenderer scglRenderer;

typedef void (*scglRenderingFunction)(uint32_t id);

scglRenderer* scglCreateRenderer();

#endif
