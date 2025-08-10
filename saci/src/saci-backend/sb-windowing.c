#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "saci-backend/sb-config-manager.h"
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
SA_INTERNAL void sb__initialize_debugger(void);

/* === Main declaration=== */

SA_INTERNAL struct sb_WindowingApiFuncs windowing_dependent_funcs;
SA_INTERNAL struct sb_RenderApiLoaderFuncs loader_dependent_funcs;
SA_INTERNAL struct sb_RenderApiFuncs render_dependent_funcs;

SA_INTERNAL enum sb_RendererApi render_api;

su_Bool sb_load_windowing(void) {
    windowing_dependent_funcs = sb_cfg_manager_get_window_funcs();
    int success = windowing_dependent_funcs.init();
    if (!success) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_INIT,
                             "Couldn't load window api");
        return su_FALSE;
    }
    su_LOG_INFO_PRINT_M(su_LOG_CONTEXT_OPENGL, "Loaded window api");

    windowing_dependent_funcs.set_hint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    windowing_dependent_funcs.set_hint(GLFW_CONTEXT_VERSION_MINOR, 3);
    windowing_dependent_funcs.set_hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return su_TRUE;
}

su_Bool sb_proc_load(void) {
    if (loader_dependent_funcs.load_opengl(windowing_dependent_funcs.get_proc()) != su_TRUE) {
        su_LOG_ERROR_PRINT_M(su_LOG_SEVERITY_HIGH, su_LOG_CONTEXT_INIT, "Couldn't load proc");
        return su_FALSE;
    }
    su_LOG_INFOF_PRINT_M(su_LOG_CONTEXT_INIT, "Loaded proc: %d", render_api);
    sb__initialize_debugger();

    // su_LOG_INFOF_PRINT_M(su_LOG_CONTEXT_OPENGL,
    //                      "Using OpenGL version: %s", render_dependent_funcs.get_version_string(GL_VERSION));
    return su_TRUE;
}

su_Window sb_window_create(int width, int height, const char* title,
                           su_Monitor monitor, su_Window share) {
    return windowing_dependent_funcs.create_window(width, height, title, monitor, share);
}

void sb_window_free(su_Window window) {
    windowing_dependent_funcs.destroy_window(window);
}

void sb_window_make_context(su_Window window) {
    windowing_dependent_funcs.make_context_current(window);
}

su_Bool sb_window_should_close(su_Window window) {
    return windowing_dependent_funcs.should_close(window);
}

void sb_window_set_pos_handler(su_Window window, su_WindowPosHandler window_pos_handler) {
    windowing_dependent_funcs.set_pos_handler(window, window_pos_handler);
    su_LOG_INFO_PRINT_M(su_LOG_CONTEXT_OPENGL, "Set window pos handler");
}

void sb_window_set_size_handler(su_Window window, su_WindowSizeHandler window_size_handler) {
    windowing_dependent_funcs.set_size_handler(window, window_size_handler);
    su_LOG_INFO_PRINT_M(su_LOG_CONTEXT_OPENGL, "Set window size callback");
}

void sb_window_terminate(void) {
    windowing_dependent_funcs.terminate();
    su_LOG_INFO_PRINT_M(su_LOG_CONTEXT_OPENGL, "Terminated glfw");
}

void sb_window_clear_color(su_Color color) {
    render_dependent_funcs.clear_color(color.r, color.g, color.b, color.a);
    render_dependent_funcs.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void sb_window_swap_buffer(su_Window window) {
    windowing_dependent_funcs.swap_buffers(window);
}

/* === Helper === */

void sb__initialize_debugger(void) {
    render_dependent_funcs.enable(GL_DEBUG_OUTPUT);
    render_dependent_funcs.enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    render_dependent_funcs.enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    render_dependent_funcs.debug_message_callback(su_log_opengl_debug_message_callback, NULL);
    su_LOG_DEBUG_PRINT_M(su_LOG_DEBUG_TYPE_OPENGL, su_LOG_CONTEXT_OPENGL, "Loaded OpenGL debugger");
}

/* === Event === */

SA_API void sb_event_poll(void) {
    windowing_dependent_funcs.poll_events();
}

SA_API void sb_event_wait(void) {
    windowing_dependent_funcs.wait_events();
}

SA_API void sb_event_wait_for_timeout(double timeout) {
    windowing_dependent_funcs.wait_events_timeout(timeout);
}

SA_API void sb_event_post_empty(void) {
    windowing_dependent_funcs.post_empty_event();
}

SA_API void sb_event_set_mouse_pos_handler(su_Window window, su_EventMousePosHandler mouse_pos_handler) {
    windowing_dependent_funcs.set_mouse_pos_handler(window, mouse_pos_handler);
}

SA_API su_Bool sb_event_is_key_pressed(su_Window window, int keycode) {
    return windowing_dependent_funcs.is_key_pressed(window, su_SCAST_TO_M(int)(keycode)) == GLFW_PRESS;
}
