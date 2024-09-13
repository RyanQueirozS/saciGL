#ifndef __SACI_SCGL_H__
#define __SACI_SCGL_H__

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

// These 3 should not be used, it's in here just in case the dev decides to
u32 scglCompileShaderV(const char* source);
u32 scglCompileShaderF(const char* source);
u32 scglCompileShaderG(const char* source);

// These should also not be accessed by the dev, but it's here for the sake of liberty
u32 scglGetShaderProgram(u32 vshader, u32 fshader);
u32 scglGetShaderProgramg(u32 vshader, u32 fshader, u32 gshader);

//----------------------------------------------------------------------------//
// Render
//----------------------------------------------------------------------------//

typedef struct scglRenderer scglRenderer;

scglRenderer* scglCreateRenderer(void);
void scglBeginComposition(scglRenderer* renderer);
void scglRenderComposition(scglRenderer* renderer);
void scglDeleteRenderer(scglRenderer* renderer);

// Renderer config
void scglRenderSetNoFillMode(void);
void scglRenderSetFillMode(void);

//----------------------------------------------------------------------------//
// Draw
//----------------------------------------------------------------------------//

void scglClearBackground(color color);
void scglRenderDrawRect(scglRenderer* renderer, const saciRect rect, color color);
void scglRenderDrawTriangle(scglRenderer* renderer, const saciTriangle triangle,
                            const color aColor, const color bColor, const color cColor);
void scglRenderDrawTriangleOneColor(scglRenderer* renderer, const saciTriangle triangle, const color onlyColor);

//----------------------------------------------------------------------------//

#endif
