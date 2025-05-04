#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "saci-core/sc-windowing.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"

/* === Helper === */

/**
 * @brief Generates the default OpenGL debuger.
 */
SA_INTERNAL void sc_OpenGL_InitializeDebugger_s(void);

/* === Main declaration=== */

sa_bool sc_GLFW_Init(void) {
    int success = glfwInit();
    if (!success) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, "Couldn't load glfw");
        return sa_FALSE;
    }
    sa_Log_Info_Print_m(sa_LOG_CONTEXT_OPENGL, "Loaded glfw");
    // TODO make user defined version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return sa_TRUE;
}

sa_bool sc_GLAD_Init(void) {
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != sa_TRUE) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_HIGH, sa_LOG_CONTEXT_OPENGL, "Couldn't Load glad");
        return sa_FALSE;
    }
    sa_Log_Info_Print_m(sa_LOG_CONTEXT_OPENGL, "Loaded glad");
    sc_OpenGL_InitializeDebugger_s();

#if 0 // TODO
    const sa_u8* version = glGetString(GL_VERSION);
    char versionStr[256];
    snprintf(versionStr, sizeof(versionStr), "Using OpenGL version: %s", version);
    sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_WINDOWING,sa_LOG_CONTEXT_OPENGL, versionStr);
#endif

    return sa_TRUE;
}

sc_window_t* sc_Window_Create(int width, int height, const char* title,
                              sc_monitor_t* monitor, sc_window_t* share) {
    return glfwCreateWindow(width, height, title, monitor, share);
}

void sc_Window_Free(sc_window_t* window) {
    glfwDestroyWindow(window);
}

void sc_Window_Make_Context(sc_window_t* window) {
    glfwMakeContextCurrent(window);
}

sa_bool sc_Window_Should_Close(sc_window_t* window) {
    return glfwWindowShouldClose(window);
}

void sc_Window_Set_Pos_Handler(sc_window_t* window, sc_window_posHandler_t windowPosHandler) {
    glfwSetWindowPosCallback(window, windowPosHandler);
    sa_Log_Info_Print_m(sa_LOG_CONTEXT_OPENGL, "Set window pos handler");
}

void sc_Window_Set_Size_Handler(sc_window_t* window, sc_window_sizeHandler_t windowSizeHandler) {
    glfwSetWindowSizeCallback(window, windowSizeHandler);
    sa_Log_Info_Print_m(sa_LOG_CONTEXT_OPENGL, "Set window size callback");
}

void sc_Window_Terminate(void) {
    glfwTerminate();
    sa_Log_Info_Print_m(sa_LOG_CONTEXT_OPENGL, "Terminated glfw");
}

void sc_Window_Clear_Color(sa_color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void sc_Window_Swap_Buffer(sc_window_t* window) {
    glfwSwapBuffers(window);
}

/* === Helper === */

void sc_OpenGL_InitializeDebugger_s(void) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(sa_Log_OpenGL_Debug_Message_Callback, NULL);
    sa_Log_Debug_Print_m(sa_LOG_DEBUG_TYPE_OPENGL, sa_LOG_CONTEXT_OPENGL, "Loaded OpenGL debugger");
}
