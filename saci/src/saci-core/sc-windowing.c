#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include "saci-core.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-debug.h"

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

void __sc_initializeOpenglDebugger();

//----------------------------------------------------------------------------//

saci_Bool sc_GLFWInit(void) {
    int success = glfwInit();
    if (!success) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_OPENGL, "Couldn't load glfw");
        return SACI_FALSE;
    }
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_OPENGL, "Loaded glfw");
    // TODO make user defined version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return SACI_TRUE;
}

saci_Bool sc_GLADInit(void) {
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != SACI_TRUE) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_OPENGL, "Couldn't Load glad");
        return SACI_FALSE;
    }
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_OPENGL, "Loaded glad");
    __sc_initializeOpenglDebugger();
    return SACI_TRUE;
}

sc_Window* sc_CreateWindow(int width, int height, const char* title,
                           sc_Monitor* monitor, sc_Window* share) {
    return glfwCreateWindow(width, height, title, monitor, share);
}
void sc_MakeWindowContext(sc_Window* window) { glfwMakeContextCurrent(window); }

saci_Bool sc_WindowShouldClose(sc_Window* window) { return glfwWindowShouldClose(window); }

void sc_SetWindowPosHandler(sc_Window* window, sc_WindowPosHandler windowPosHandler) {
    glfwSetWindowPosCallback(window, windowPosHandler);
}

void sc_SetWindowSizeHandler(sc_Window* window, sc_WindowSizeHandler windowSizeHandler) {
    glfwSetWindowSizeCallback(window, windowSizeHandler);
}

void sc_Terminate(void) {
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_OPENGL, "Terminated glfw");
    glfwTerminate();
}

void sc_ClearWindow(saci_Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void sc_SwapWindowBuffer(sc_Window* window) {
    glfwSwapBuffers(window);
}

//----------------------------------------------------------------------------//
// Helper functions
//----------------------------------------------------------------------------//

void __sc_initializeOpenglDebugger() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(saci_OpenGLDebugMessageCallback, NULL);
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_OPENGL, "Loaded OpenGL debugger");
}
