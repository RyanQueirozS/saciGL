#include "saci_platform/windowing/windowing.h"

#include "../dependencies/internal/dependency.h"

#include "saci_util/internal/log.h"
#include "saci_util/log.h"
#include "saci_util/internal/general.h"

#include "saci_platform/gfx/internal/gfx.h"
#include <stdio.h>

#ifdef __EMSCRIPTEN__
#  include "./internal/emsdk.h"
#else
#  include "./internal/glfw.h"
#endif

/* === Internal Funcs === */

#ifndef __EMSCRIPTEN__
SACI_INTERNAL int psaci__windowing_init(void);

SACI_INTERNAL void* psaci__windowing_get_proc(void);
#endif

/* === Main declaration=== */

SaciBool psaci_window_load(void)
{
    // Emscripten doesn't need to load any api at runtime, nor any proc
#ifndef __EMSCRIPTEN__
    psaci_window_glfw_load_dependencies(psaci_dependencies_get_windowing_api_funcs());

    int success = psaci__windowing_init();
    if (!success) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_PROD, SACI_LOG_ERROR_SEVERITY_CRASH, SACI_LOG_CONTEXT_WINDOWING,
                         "Couldn't load window api");
        return SACI_FALSE;
    }
    SACI_LOG_INFO_M(SACI_LOG_TYPE_PROD, SACI_LOG_CONTEXT_WINDOWING, "Loaded window api");

    psaci_gfx_load_proc(SACI_CAST_M(void* (*)(const char*)) psaci__windowing_get_proc());
#endif
    return SACI_TRUE;
}

union PSaciWindow psaci_window_create(int width, int height, const char* title,
                                      union PSaciWindowOpts opts)
{
#ifdef __EMSCRIPTEN__
    (void)title;
    return sb_window_emscripten_create(width, height, opts);
#else
    return psaci_window_glfw_create(width, height, title, opts);
#endif
}

void psaci_window_free(union PSaciWindow window)
{
#ifdef __EMSCRIPTEN__
    sb_window_emscripten_free(window);
#else
    psaci_window_glfw_free(window);
#endif
}

void psaci_window_make_context(union PSaciWindow window)
{
#ifdef __EMSCRIPTEN__
    sb_window_emscripten_make_context(window);
#else
    psaci_window_glfw_make_context(window);
#endif
    SACI_LOG_INFO_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_WINDOWING, "Making context current");
}

SaciBool psaci_window_should_close(union PSaciWindow window)
{
    SaciBool should_close = SACI_FALSE;
#ifdef __EMSCRIPTEN__
    should_close = sb_window_emscripten_should_close(window);
#else
    should_close = psaci_window_glfw_should_close(window);
#endif
    return should_close;
}

void psaci_window_set_pos_handler(union PSaciWindow window, PSaciWindowPosHandler window_pos_handler)
{
#ifdef __EMSCRIPTEN__
    (void)window, (void)window_pos_handler;
#else
    psaci_window_glfw_set_pos_handler(window, (void*)window_pos_handler); // TODO
#endif
    SACI_LOG_INFO_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_WINDOWING, "Set pos handler");
}

void psaci_window_set_size_handler(union PSaciWindow window, PSaciWindowSizeHandler window_size_handler)
{
#ifdef __EMSCRIPTEN__
    sb_window_emscripten_set_size_handler(window, window_size_handler);
#else
    psaci_window_glfw_set_size_handler(window, (void*)window_size_handler); // TODO
#endif
    SACI_LOG_INFO_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_WINDOWING, "Set window size handler");
}

void psaci_window_terminate(void)
{
#ifndef __EMSCRIPTEN__
    psaci_window_glfw_terminate();
#endif
    SACI_LOG_INFO_M(SACI_LOG_TYPE_USER, SACI_LOG_CONTEXT_WINDOWING, "Terminated window");
}

void psaci_window_swap_buffer(union PSaciWindow window)
{
#ifdef __EMSCRIPTEN__
    (void)window;
#else
    psaci_window_glfw_swap_buffer(window);
#endif
}

/* === Event === */

void psaci_event_poll(void)
{
#ifndef __EMSCRIPTEN__
    psaci_event_glfw_poll();
#endif
}

void psaci_event_wait(void)
{
#ifndef __EMSCRIPTEN__
    psaci_event_glfw_wait();
#endif
}

void psaci_event_wait_for_timeout(double timeout)
{
#ifdef __EMSCRIPTEN__
    sb_event_emscripten_wait_for_timeout(timeout);
#else
    psaci_event_glfw_wait_for_timeout(timeout);
#endif
}

void psaci_event_post_empty(void)
{
#ifdef __EMSCRIPTEN__
    sb_event_emscripten_post_empty();
#else
    psaci_event_glfw_post_empty();
#endif
}

void psaci_event_set_mouse_pos_handler(union PSaciWindow window, SaciEventMousePosHandler mouse_pos_handler)
{
#ifdef __EMSCRIPTEN__
    sb_event_emscripten_set_mouse_pos_handler(window, mouse_pos_handler);
#else
    psaci_event_glfw_set_mouse_pos_handler(window, mouse_pos_handler);
#endif
}

SaciBool psaci_event_is_key_pressed(union PSaciWindow window, int keycode)
{
#ifdef __EMSCRIPTEN__
    (void)window;
    return sb_event_emscripten_is_key_pressed(keycode);
#else
    return psaci_event_glfw_is_key_pressed(window, keycode);
#endif
}

/* === Internal Funcs === */

#ifndef __EMSCRIPTEN__
int psaci__windowing_init(void)
{
    int status = 0;
    status = psaci_window_glfw_init();
    return status;
}

void* psaci__windowing_get_proc(void)
{
    return psaci_window_glfw_get_proc();
}
#endif
