#include "saci-backend/windowing/sb-windowing.h"

#include "../resources/sb-dependency-internal.h"
#include "saci-backend/graphics/sb-gfx.h"
#include "saci-utils/su-log.h"

#ifdef __EMSCRIPTEN__
#else
#  include "./sb-glfw-internal.h"
#endif

/* === Internal Funcs === */

SA_INTERNAL int sb__windowing_init(void);

SA_INTERNAL void* sb__windowing_get_proc(void);

/* === Main declaration=== */

SA_INTERNAL struct sb_WindowingApiFuncs sb__windowing_dependent_funcs;

su_Bool sb_window_load(void)
{
    sb__windowing_dependent_funcs = sb_dependencies_get_windowing_api_funcs();

#ifdef __EMSCRIPTEN__
#else
    sb_window_glfw_load_dependencies(sb__windowing_dependent_funcs);
#endif

    int success = sb__windowing_init();
    if (!success) {
        su_LOG_ERROR_M(su_LOG_TYPE_PROD, su_LOG_ERROR_SEVERITY_CRASH, su_LOG_CONTEXT_WINDOWING,
                       "Couldn't load window api");
        return su_FALSE;
    }
    su_LOG_INFO_M(su_LOG_TYPE_PROD, su_LOG_CONTEXT_WINDOWING, "Loaded window api");

    return su_TRUE;
}

su_Bool sb_window_proc_load(void)
{
    sb_gfx_load_proc(su_CAST_M(void* (*)(const char*)) sb__windowing_get_proc());
    return su_TRUE;
}

su_Window sb_window_create(int width, int height, const char* title,
                           su_Monitor monitor, su_Window share)
{
#ifdef __EMSCRIPTEN__
#else
    return sb_window_glfw_create(width, height, title, monitor, share);
#endif
}

void sb_window_free(su_Window window)
{
#ifdef __EMSCRIPTEN__
#else
    sb_window_glfw_free(window);
#endif
}

void sb_window_make_context(su_Window window)
{
#ifdef __EMSCRIPTEN__
#else
    sb_window_glfw_make_context(window);
#endif
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_WINDOWING, "Making context current");
}

su_Bool sb_window_should_close(su_Window window)
{
    su_Bool should_close = su_FALSE;
#ifdef __EMSCRIPTEN__
#else
    should_close = sb_window_glfw_should_close(window);
#endif
    return should_close;
}

void sb_window_set_pos_handler(su_Window window, su_WindowPosHandler window_pos_handler)
{
#ifdef __EMSCRIPTEN__
#else
    sb_window_glfw_set_pos_handler(window, window_pos_handler);
#endif
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_WINDOWING, "Set pos handler");
}

void sb_window_set_size_handler(su_Window window, su_WindowSizeHandler window_size_handler)
{
#ifdef __EMSCRIPTEN__
#else
    sb_window_glfw_set_size_handler(window, window_size_handler);
#endif
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_WINDOWING, "Set window size handler");
}

void sb_window_terminate(void)
{
#ifdef __EMSCRIPTEN__
#else
    sb_window_glfw_terminate();
#endif
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_WINDOWING, "Terminated GLFW");
}

void sb_window_swap_buffer(su_Window window)
{
#ifdef __EMSCRIPTEN__
#else
    sb_window_glfw_swap_buffer(window);
#endif
}

/* === Event === */

void sb_event_poll(void)
{
#ifdef __EMSCRIPTEN__
#else
    sb_event_glfw_poll();
#endif
}

void sb_event_wait(void)
{
#ifdef __EMSCRIPTEN__
#else
    sb_event_glfw_wait();
#endif
}

void sb_event_wait_for_timeout(double timeout)
{
#ifdef __EMSCRIPTEN__
#else
    sb_event_glfw_wait_for_timeout(timeout);
#endif
}

void sb_event_post_empty(void)
{
#ifdef __EMSCRIPTEN__
#else
    sb_event_glfw_post_empty();
#endif
}

void sb_event_set_mouse_pos_handler(su_Window window, su_EventMousePosHandler mouse_pos_handler)
{
#ifdef __EMSCRIPTEN__
#else
    sb_event_glfw_set_mouse_pos_handler(window, mouse_pos_handler);
#endif
}

su_Bool sb_event_is_key_pressed(su_Window window, int keycode)
{
#ifdef __EMSCRIPTEN__
#else
    return sb_event_glfw_is_key_pressed(window, keycode);
#endif
}

/* === Internal Funcs === */

int sb__windowing_init(void)
{
    int status = 0;
#ifdef __EMSCRIPTEN__
#else
    status = sb_window_glfw_init();
#endif
    return status;
}

void* sb__windowing_get_proc(void)
{
#ifdef __EMSCRIPTEN
#else
    return (void*)sb_window_glfw_get_proc;
#endif
}
