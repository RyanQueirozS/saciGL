#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "saci-core/sc-gl.h"
#include "saci-utils/su-debug.h"

/* === Helper === */

/**
 * @brief Generates the default OpenGL debuger.
 */
void __sc_OpenGL_InitializeDebugger(void);

/* === Main declaration=== */

sa_bool_t sc_GLFW_Init(void) {
    int success = glfwInit();
    if (!success) {
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, "Couldn't load glfw");
        return sa_FALSE;
    }
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_OPENGL, "Loaded glfw");
    // TODO make user defined version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return sa_TRUE;
}

sa_bool_t sc_GLAD_Init(void) {
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != sa_TRUE) {
        sa_LOG_ERROR_PRINT_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, "Couldn't Load glad");
        return sa_FALSE;
    }
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_OPENGL, "Loaded glad");
    __sc_OpenGL_InitializeDebugger();

#if defined(SACI_DEBUG_MODE) || defined(SACI_DEBUG_MODE_WINDOWING)
    const sa_u8_t* version = glGetString(GL_VERSION);
    char versionStr[256];
    snprintf(versionStr, sizeof(versionStr), "Using OpenGL version: %s", version);
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_DEBUG, sa_LOG_CONTEXT_OPENGL, versionStr);
#endif

    return sa_TRUE;
}

sc_Window_t* sc_Window_Create(int width, int height, const char* title,
                              sc_Monitor_t* monitor, sc_Window_t* share) {
    return glfwCreateWindow(width, height, title, monitor, share);
}

void sc_Window_Free(sc_Window_t* window) {
    glfwDestroyWindow(window);
}

void sc_Window_Make_Context(sc_Window_t* window) {
    glfwMakeContextCurrent(window);
}

sa_bool_t sc_Window_Should_Close(sc_Window_t* window) {
    return glfwWindowShouldClose(window);
}

void sc_Window_Set_Pos_Handler(sc_Window_t* window, sc_Window_PosHandler_t windowPosHandler) {
    glfwSetWindowPosCallback(window, windowPosHandler);
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_OPENGL, "Set window pos handler");
}

void sc_Window_Set_Size_Handler(sc_Window_t* window, sc_Window_SizeHandler_t windowSizeHandler) {
    glfwSetWindowSizeCallback(window, windowSizeHandler);
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_OPENGL, "Set window size callback");
}

void sc_Window_Terminate(void) {
    glfwTerminate();
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_INFO, sa_LOG_CONTEXT_OPENGL, "Terminated glfw");
}

void sc_Window_Clear_Color(sa_Color_t color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void sc_Window_Swap_Buffer(sc_Window_t* window) {
    glfwSwapBuffers(window);
}

/* === Helper === */

void __sc_OpenGL_InitializeDebugger() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(sa_OpenGL_Debug_Message_Callback, NULL);
#ifdef SACI_DEBUG_MODE
    sa_LOG_INFO_PRINT_m(sa_LOG_TYPE_DEBUG, sa_LOG_CONTEXT_OPENGL, "Loaded OpenGL debugger");
#endif
}
