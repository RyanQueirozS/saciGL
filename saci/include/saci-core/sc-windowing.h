#ifndef __SACI_CORE_SC_WINDOWING_H__
#define __SACI_CORE_SC_WINDOWING_H__

#include <GLFW/glfw3.h>
#include "saci-utils/su-types.h"

typedef GLFWmonitor sc_Monitor;
typedef GLFWwindow sc_Window;

//----------------------------------------------------------------------------//
// Windowing
//----------------------------------------------------------------------------//

saci_Bool sc_GLFWInit(void);
saci_Bool sc_GLADInit(void);

// Creates a window, does not check if it's null
sc_Window* sc_CreateWindow(int width, int height, const char* title,
                           sc_Monitor* monitor, sc_Window* share);
void sc_MakeWindowContext(sc_Window* window);

saci_Bool sc_WindowShouldClose(sc_Window* window);

typedef void (*sc_WindowPosHandler)(sc_Window* window, int width, int height);
void sc_SetWindowPosHandler(sc_Window* window, sc_WindowPosHandler windowPosHandler);
typedef void (*sc_WindowSizeHandler)(sc_Window* window, int posx, int posy);
void sc_SetWindowSizeHandler(sc_Window* window, sc_WindowSizeHandler windowSizeHandler);

void sc_Terminate(void);

void sc_ClearWindow(const saci_Color color);

void sc_SwapWindowBuffer(sc_Window* window);

#endif
