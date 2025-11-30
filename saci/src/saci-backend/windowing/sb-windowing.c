#include "saci-backend/windowing/sb-windowing.h"

#include "../resources/sb-dependency-internal.h"
#include "saci-backend/graphics/sb-gfx.h"
#include "saci-utils/su-log.h"

#ifdef __EMSCRIPTEN__
#  include "./sb-emsdk-internal.h"
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

#ifndef __EMSCRIPTEN__
    // Emscripten doesn't need to load dependencies at runtime
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
#ifdef __EMSCRIPTEN__
    // Doesn't need to load proc
    return su_TRUE;
#else
    sb_gfx_load_proc(su_CAST_M(void* (*)(const char*)) sb__windowing_get_proc());
    return su_TRUE;
#endif
}

union sb_Window sb_window_create(int width, int height, const char* title,
                                 union sb_WindowOpts opts)
{
#ifdef __EMSCRIPTEN__
    return sb_window_emscripten_create(width, height, title, opts);
#else
    return sb_window_glfw_create(width, height, title, opts);
#endif
}

void sb_window_free(union sb_Window window)
{
#ifdef __EMSCRIPTEN__
    sb_window_emscripten_free(window);
#else
    sb_window_glfw_free(window);
#endif
}

void sb_window_make_context(union sb_Window window)
{
#ifdef __EMSCRIPTEN__
    sb_window_emscripten_make_context(window);
#else
    sb_window_glfw_make_context(window);
#endif
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_WINDOWING, "Making context current");
}

su_Bool sb_window_should_close(union sb_Window window)
{
    su_Bool should_close = su_FALSE;
#ifdef __EMSCRIPTEN__
    should_close = sb_window_emscripten_should_close(window);
#else
    should_close = sb_window_glfw_should_close(window);
#endif
    return should_close;
}

void sb_window_set_pos_handler(union sb_Window window, su_WindowPosHandler window_pos_handler)
{
#ifdef __EMSCRIPTEN__
    sb_window_emscripten_set_pos_handler(window, window_pos_handler);
#else
    sb_window_glfw_set_pos_handler(window, window_pos_handler);
#endif
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_WINDOWING, "Set pos handler");
}

void sb_window_set_size_handler(union sb_Window window, su_WindowSizeHandler window_size_handler)
{
#ifdef __EMSCRIPTEN__
    sb_window_emscripten_set_size_handler(window, window_size_handler);
#else
    sb_window_glfw_set_size_handler(window, window_size_handler);
#endif
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_WINDOWING, "Set window size handler");
}

void sb_window_terminate(void)
{
#ifdef __EMSCRIPTEN__
    sb_window_emscripten_terminate();
#else
    sb_window_glfw_terminate();
#endif
    su_LOG_INFO_M(su_LOG_TYPE_USER, su_LOG_CONTEXT_WINDOWING, "Terminated GLFW");
}

void sb_window_swap_buffer(union sb_Window window)
{
#ifdef __EMSCRIPTEN__
    sb_window_emscripten_swap_buffer(window);
#else
    sb_window_glfw_swap_buffer(window);
#endif
}

/* === Event === */

void sb_event_poll(void)
{
#ifdef __EMSCRIPTEN__
    sb_event_emscripten_poll();
#else
    sb_event_glfw_poll();
#endif
}

void sb_event_wait(void)
{
#ifdef __EMSCRIPTEN__
    sb_event_emscripten_wait();
#else
    sb_event_glfw_wait();
#endif
}

void sb_event_wait_for_timeout(double timeout)
{
#ifdef __EMSCRIPTEN__
    sb_event_emscripten_wait_for_timeout(timeout);
#else
    sb_event_glfw_wait_for_timeout(timeout);
#endif
}

void sb_event_post_empty(void)
{
#ifdef __EMSCRIPTEN__
    sb_event_emscripten_post_empty();
#else
    sb_event_glfw_post_empty();
#endif
}

void sb_event_set_mouse_pos_handler(union sb_Window window, su_EventMousePosHandler mouse_pos_handler)
{
#ifdef __EMSCRIPTEN__
    sb_event_emscripten_set_mouse_pos_handler(window, mouse_pos_handler);
#else
    sb_event_glfw_set_mouse_pos_handler(window, mouse_pos_handler);
#endif
}

su_Bool sb_event_is_key_pressed(union sb_Window window, int keycode)
{
#ifdef __EMSCRIPTEN__
    return sb_event_emscripten_is_key_pressed(window, keycode);
#else
    return sb_event_glfw_is_key_pressed(window, keycode);
#endif
}

/* === Internal Funcs === */

int sb__windowing_init(void)
{
    int status = 0;
#ifdef __EMSCRIPTEN__
    status = sb_window_emscripten_init();
#else
    status = sb_window_glfw_init();
#endif
    return status;
}

void* sb__windowing_get_proc(void)
{
#ifdef __EMSCRIPTEN__
    return NULL;
#else
    return sb_window_glfw_get_proc();
#endif
}
