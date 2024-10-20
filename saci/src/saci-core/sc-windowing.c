#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <stdio.h>
#include "saci-core.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-debug.h"

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

void __sc_OpenGL_initializeDebugger();

//----------------------------------------------------------------------------//

saci_Bool sc_GLFW_Init(void) {
    int success = glfwInit();
    if (!success) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_OPENGL,
                       "Couldn't load glfw");
        return SACI_FALSE;
    }
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_OPENGL, "Loaded glfw");
    // TODO make user defined version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return SACI_TRUE;
}

saci_Bool sc_GLAD_Init(void) {
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != SACI_TRUE) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_OPENGL,
                       "Couldn't Load glad");
        return SACI_FALSE;
    }
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_OPENGL, "Loaded glad");
    __sc_OpenGL_initializeDebugger();

#if defined(SACI_DEBUG_MODE) || defined(SACI_DEBUG_MODE_WINDOWING)
    const saci_u8* version = glGetString(GL_VERSION);
    char versionStr[256];
    snprintf(versionStr, sizeof(versionStr), "Using OpenGL version: %s", version);
    SACI_LOG_PRINT(SACI_LOG_LEVEL_DEBUG, SACI_LOG_CONTEXT_OPENGL, versionStr);
#endif

    return SACI_TRUE;
}

sc_Window* sc_Window_Create(int width, int height, const char* title, sc_Monitor* monitor,
                            sc_Window* share) {
    return glfwCreateWindow(width, height, title, monitor, share);
}
void sc_Window_MakeContext(sc_Window* window) { glfwMakeContextCurrent(window); }

saci_Bool sc_Window_ShouldClose(sc_Window* window) {
    return glfwWindowShouldClose(window);
}

void sc_Window_SetPosHandler(sc_Window* window, sc_Window_PosHandler windowPosHandler) {
    glfwSetWindowPosCallback(window, windowPosHandler);
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_OPENGL,
                   "Set window pos handler");
}

void sc_Window_SetSizeHandler(sc_Window* window,
                              sc_Window_SizeHandler windowSizeHandler) {
    glfwSetWindowSizeCallback(window, windowSizeHandler);
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_OPENGL,
                   "Set window size callback");
}

void sc_Window_Terminate(void) {
    glfwTerminate();
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_OPENGL, "Terminated glfw");
}

void sc_Window_ClearColor(saci_Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void sc_Window_SwapBuffer(sc_Window* window) { glfwSwapBuffers(window); }

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

void __sc_OpenGL_initializeDebugger() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(saci_OpenGLDebugMessageCallback, NULL);
#ifdef SACI_DEBUG_MODE
    SACI_LOG_PRINT(SACI_LOG_LEVEL_DEBUG, SACI_LOG_CONTEXT_OPENGL,
                   "Loaded OpenGL debugger");
#endif
}
