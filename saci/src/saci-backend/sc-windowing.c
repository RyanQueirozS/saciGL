#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "saci-backend/sc-windowing.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"

/* === Helper === */

/**
 * @brief Generates the default OpenGL debuger.
 */
SA_INTERNAL void s_Opengl_Initialize_Debugger(void);

/* === Main declaration=== */

sa_bool sc_GLFW_Init(void) {
    int success = glfwInit();
    if (!success) {
        su_Log_Error_Print_m(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_OPENGL, "Couldn't load glfw");
        return sa_FALSE;
    }
    su_Log_Info_Print_m(su_LOG_CONTEXT_OPENGL, "Loaded glfw");
    // TODO make user defined version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return sa_TRUE;
}

sa_bool sc_GLAD_Init(void) {
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != sa_TRUE) {
        su_Log_Error_Print_m(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_OPENGL, "Couldn't Load glad");
        return sa_FALSE;
    }
    su_Log_Info_Print_m(su_LOG_CONTEXT_OPENGL, "Loaded glad");
    s_Opengl_Initialize_Debugger();

    su_Log_InfoF_Print_m(su_LOG_CONTEXT_OPENGL,
                         "Using OpenGL version: %s", glGetString(GL_VERSION));

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

void sc_Window_Set_Pos_Handler(sc_window_t* window, sc_window_posHandler_t window_pos_handler) {
    glfwSetWindowPosCallback(window, window_pos_handler);
    su_Log_Info_Print_m(su_LOG_CONTEXT_OPENGL, "Set window pos handler");
}

void sc_Window_Set_Size_Handler(sc_window_t* window, sc_window_sizeHandler_t window_size_handler) {
    glfwSetWindowSizeCallback(window, window_size_handler);
    su_Log_Info_Print_m(su_LOG_CONTEXT_OPENGL, "Set window size callback");
}

void sc_Window_Terminate(void) {
    glfwTerminate();
    su_Log_Info_Print_m(su_LOG_CONTEXT_OPENGL, "Terminated glfw");
}

void sc_Window_Clear_Color(sa_color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void sc_Window_Swap_Buffer(sc_window_t* window) {
    glfwSwapBuffers(window);
}

/* === Helper === */

void s_Opengl_Initialize_Debugger(void) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(su_Log_OpenGL_Debug_Message_Callback, NULL);
    su_Log_Debug_Print_m(su_LOG_DEBUG_TYPE_OPENGL, su_LOG_CONTEXT_OPENGL, "Loaded OpenGL debugger");
}

/* === Event === */

SA_API void sc_Event_Poll(void) {
    glfwPollEvents();
}

SA_API void sc_Event_Wait(void) {
    glfwWaitEvents();
}

SA_API void sc_Event_Wait_For_Timeout(double timeout) {
    glfwWaitEventsTimeout(timeout);
}

SA_API void sc_Event_Post_Empty(void) {
    glfwPostEmptyEvent();
}

SA_API void sc_Event_Set_Mouse_Pos_Handler(sc_window_t* window, sc_event_mousePosHandler_t mouse_pos_handler) {
    glfwSetCursorPosCallback(window, mouse_pos_handler);
}

SA_API sa_bool sc_Event_Is_Key_Pressed(sc_window_t* window, int keycode) {
    return glfwGetKey(window, su_Scast_To_m(int)(keycode)) == GLFW_PRESS;
}
