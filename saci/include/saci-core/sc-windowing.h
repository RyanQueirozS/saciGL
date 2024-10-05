#ifndef __SACI_CORE_SC_WINDOWING_H__
#define __SACI_CORE_SC_WINDOWING_H__

#include <GLFW/glfw3.h>
#include "saci-utils/su-types.h"

typedef GLFWmonitor saciGL_Monitor;
typedef GLFWwindow saciGL_Window;

//----------------------------------------------------------------------------//
// Windowing
//----------------------------------------------------------------------------//

saci_Bool saciGL_GLFWInit(void);
saci_Bool saciGL_GLEWInit(void);

// Creates a window, does not check if it's null
saciGL_Window* saciGL_CreateWindow(int width, int height, const char* title,
                                   saciGL_Monitor* monitor, saciGL_Window* share);
void saciGL_MakeWindowContext(saciGL_Window* window);

typedef void (*saciGL__WindowPosHandler)(saciGL_Window* window, int width, int height);
void saciGL_SetWindowPosHandler(saciGL_Window* window, saciGL__WindowPosHandler windowPosHandler);
typedef void (*saciGL__WindowSizeHandler)(saciGL_Window* window, int posx, int posy);
void saciGL_SetWindowSizeHandler(saciGL_Window* window, saciGL__WindowSizeHandler windowSizeHandler);

void saciGL_Terminate(void);

void saciGL_ClearWindow(const saci_Color color);

void saciGL_SwapWindowBuffer(saciGL_Window* window);

#endif
