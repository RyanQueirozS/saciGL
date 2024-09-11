#ifndef __SACI_SGL_H__
#define __SACI_SGL_H__

#include <stdbool.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdint.h>

#include "helpers.h"
#include "shapes.h"

//----------------------------------------------------------------------------//

typedef GLFWmonitor scglMonitor;
typedef GLFWwindow scglWindow;

//----------------------------------------------------------------------------//
// Init
//----------------------------------------------------------------------------//

int scglGLFWInit(void);
int scglGLEWInit(void);

//----------------------------------------------------------------------------//
// Windowing
//----------------------------------------------------------------------------//

scglWindow* scglCreateWindow(int width, int height, const char* title,
                             scglMonitor* monitor, scglWindow* share);
void scglMakeWindowContext(scglWindow* window);

//----------------------------------------------------------------------------//
// Shadering
//----------------------------------------------------------------------------//

u32 scglCompileShaderV(const char* source);
u32 scglCompileShaderF(const char* source);
u32 scglCompileShaderG(const char* source);

u32 scglGetShaderProgram(u32 vshader, u32 fshader);
u32 scglGetShaderProgramg(u32 vshader, u32 fshader, u32 gshader);

//----------------------------------------------------------------------------//
// Render
//----------------------------------------------------------------------------//

void scglClearBackground(color color);

typedef struct scglRenderer scglRenderer;

scglRenderer* scglCreateRenderer(void);
void scglBeginComposition(scglRenderer* renderer);
void scglRenderComposition(scglRenderer* renderer);
void scglDeleteRenderer(scglRenderer* renderer);

//----------------------------------------------------------------------------//
// Draw
//----------------------------------------------------------------------------//

void scglRenderDrawRect(scglRenderer* renderer, const saciRect rect, color color);

#endif
