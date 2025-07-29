#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "saci-backend/sb-windowing.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-types.h"

#include <stdio.h>
#include <stdlib.h>

/* === Helper === */

/**
 * @brief Generates the default OpenGL debuger.
 */
SA_INTERNAL void sb__opengl_initialize_debugger(void);

/* === Main declaration=== */

su_Bool sb_glfw_init(void) {
    int success = glfwInit();
    if (!success) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_OPENGL, "Couldn't load glfw");
        return su_FALSE;
    }
    su_LOG_INFO_PRINT_M(su_LOG_CONTEXT_OPENGL, "Loaded glfw");
    // TODO make user defined version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return su_TRUE;
}

su_Bool sb_glad_init(void) {
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != su_TRUE) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_OPENGL, "Couldn't Load glad");
        return su_FALSE;
    }
    su_LOG_INFO_PRINT_M(su_LOG_CONTEXT_OPENGL, "Loaded glad");
    sb__opengl_initialize_debugger();

    su_LOG_INFOF_PRINT_M(su_LOG_CONTEXT_OPENGL,
                         "Using OpenGL version: %s", glGetString(GL_VERSION));

    return su_TRUE;
}

sb_Window* sb_window_create(int width, int height, const char* title,
                            sb_Monitor* monitor, sb_Window* share) {
    return glfwCreateWindow(width, height, title, monitor, share);
}

void sb_window_free(sb_Window* window) {
    glfwDestroyWindow(window);
}

void sb_window_make_context(sb_Window* window) {
    glfwMakeContextCurrent(window);
}

su_Bool sb_window_should_close(sb_Window* window) {
    return glfwWindowShouldClose(window);
}

void sb_window_set_pos_handler(sb_Window* window, sb_WindowPosHandler window_pos_handler) {
    glfwSetWindowPosCallback(window, window_pos_handler);
    su_LOG_INFO_PRINT_M(su_LOG_CONTEXT_OPENGL, "Set window pos handler");
}

void sb_window_set_size_handler(sb_Window* window, sb_WindowSizeHandler window_size_handler) {
    glfwSetWindowSizeCallback(window, window_size_handler);
    su_LOG_INFO_PRINT_M(su_LOG_CONTEXT_OPENGL, "Set window size callback");
}

void sb_window_terminate(void) {
    glfwTerminate();
    su_LOG_INFO_PRINT_M(su_LOG_CONTEXT_OPENGL, "Terminated glfw");
}

void sb_window_clear_color(su_Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void sb_window_swap_buffer(sb_Window* window) {
    glfwSwapBuffers(window);
}

/* === Helper === */

void sb__opengl_initialize_debugger(void) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(su_log_opengl_debug_message_callback, NULL);
    su_LOG_DEBUG_PRINT_M(su_LOG_DEBUG_TYPE_OPENGL, su_LOG_CONTEXT_OPENGL, "Loaded OpenGL debugger");
}

/* === Event === */

SA_API void sb_event_poll(void) {
    glfwPollEvents();
}

SA_API void sb_event_wait(void) {
    glfwWaitEvents();
}

SA_API void sb_event_wait_for_timeout(double timeout) {
    glfwWaitEventsTimeout(timeout);
}

SA_API void sb_event_post_empty(void) {
    glfwPostEmptyEvent();
}

SA_API void sb_event_set_mouse_pos_handler(sb_Window* window, sb_EventMousePosHandler mouse_pos_handler) {
    glfwSetCursorPosCallback(window, mouse_pos_handler);
}

SA_API su_Bool sb_event_is_key_pressed(sb_Window* window, int keycode) {
    return glfwGetKey(window, su_SCAST_TO_M(int)(keycode)) == GLFW_PRESS;
}
