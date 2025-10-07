#include "saci-backend/windowing/sb-windowing.h"
#include "saci-backend/graphics/sb-gfx.h"

/* === Main declaration=== */

SA_INTERNAL struct sb_WindowingApiFuncs windowing_dependent_funcs;

su_Bool sb_load_windowing(void) {
    // windowing_dependent_funcs = su_cfg_manager_get_window_funcs();
    // loader_dependent_funcs = su_cfg_manager_get_loader_funcs();
    // render_dependent_funcs = su_cfg_manager_get_render_funcs();

    int success = windowing_dependent_funcs.glfw.init();
    if (!success) {
        su_LOG_ERROR_M(su_LOG_TYPE_PROD, su_LOG_ERROR_SEVERITY_HIGH, su_LOG_CONTEXT_LIB_GLFW,
                       "Couldn't load window api");
        return su_FALSE;
    }
    su_LOG_INFO_M(su_LOG_TYPE_PROD, su_LOG_CONTEXT_LIB_GLFW, "Loaded window api");

    windowing_dependent_funcs.glfw.set_hint(sb_GLFW_CONTEXT_VERSION_MAJOR, 3);
    windowing_dependent_funcs.glfw.set_hint(sb_GLFW_CONTEXT_VERSION_MINOR, 3);
    windowing_dependent_funcs.glfw.set_hint(sb_GLFW_OPENGL_PROFILE, sb_GLFW_OPENGL_CORE_PROFILE);

    return su_TRUE;
}

// TODO
su_Bool sb_proc_load(void) {
    // if (loader_dependent_funcs.gl.load_opengl(windowing_dependent_funcs.glfw.get_proc) != su_TRUE) {
    //     su_LOG_ERROR_M(su_LOG_TYPE_PROD, su_LOG_ERROR_SEVERITY_CRASH, su_LOG_CONTEXT_LIB_GLFW, "Couldn't load proc");
    //     return su_FALSE;
    // }
    // su_LOG_INFOF_M(su_LOG_TYPE_PROD, su_LOG_CONTEXT_LIB_GLFW, "Loaded proc: %d", render_api);
    sb_gfx_load_proc(windowing_dependent_funcs.glfw.get_proc);
    return su_TRUE;
}

su_Window sb_window_create(int width, int height, const char* title,
                           su_Monitor monitor, su_Window share) {
    return windowing_dependent_funcs.glfw.create_window(width, height, title, monitor, share);
}

void sb_window_free(su_Window window) {
    windowing_dependent_funcs.glfw.destroy_window(window);
}

void sb_window_make_context(su_Window window) {
    windowing_dependent_funcs.glfw.make_context_current(window);
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_LIB_GLFW, "Making context current");
}

su_Bool sb_window_should_close(su_Window window) {
    return windowing_dependent_funcs.glfw.should_close(window);
}

void sb_window_set_pos_handler(su_Window window, su_WindowPosHandler window_pos_handler) {
    windowing_dependent_funcs.glfw.set_pos_handler(window, window_pos_handler);
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_LIB_GLFW, "Set pos handler");
}

void sb_window_set_size_handler(su_Window window, su_WindowSizeHandler window_size_handler) {
    windowing_dependent_funcs.glfw.set_size_handler(window, window_size_handler);
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_LIB_GLFW, "Set window size handler");
}

void sb_window_terminate(void) {
    windowing_dependent_funcs.glfw.terminate();
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_LIB_GLFW, "Terminated GLFW");
}

void sb_window_swap_buffer(su_Window window) {
    windowing_dependent_funcs.glfw.swap_buffers(window);
}

/* === Event === */

SA_API void sb_event_poll(void) {
    windowing_dependent_funcs.glfw.poll_events();
}

SA_API void sb_event_wait(void) {
    windowing_dependent_funcs.glfw.wait_events();
}

SA_API void sb_event_wait_for_timeout(double timeout) {
    windowing_dependent_funcs.glfw.wait_events_timeout(timeout);
}

SA_API void sb_event_post_empty(void) {
    windowing_dependent_funcs.glfw.post_empty_event();
}

SA_API void sb_event_set_mouse_pos_handler(su_Window window, su_EventMousePosHandler mouse_pos_handler) {
    windowing_dependent_funcs.glfw.set_mouse_pos_handler(window, (void*)mouse_pos_handler);
}

SA_API su_Bool sb_event_is_key_pressed(su_Window window, int keycode) {
    return windowing_dependent_funcs.glfw.is_key_pressed(window, su_CAST_M(int)(keycode)) == sb_GLFW_PRESS;
}
