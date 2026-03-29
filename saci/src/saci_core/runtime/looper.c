#include "saci_core/runtime/looper.h"
#include "saci_util/types.h"
#include "saci_core/runtime/windowing.h"

#ifdef __EMSCRIPTEN__
#  include "saci_platform/gfx/internal/gles3.h"
#endif

// Internal funcs

SACI_INTERNAL CSaciLoopFrameData csaci__looper_get_frame_data(void);

SACI_INTERNAL void csaci__looper_main_loop_wrapper(void);

// Header impl

SACI_INTERNAL struct {
    CSaciLoopFunc loop_func;
    CSaciLoopOpts opts;
    SaciBool is_running;
} csaci_g_looper_data = {0};

void csaci_looper_set_main_loop(CSaciLoopFunc loop_func, CSaciLoopOpts loop_opts)
{
    csaci_g_looper_data.loop_func = loop_func;
    csaci_g_looper_data.opts = loop_opts;
}

void csaci_looper_run(void)
{
    if (csaci_g_looper_data.is_running) {
        return;
    }

    csaci_g_looper_data.is_running = SACI_TRUE;
#ifndef __EMSCRIPTEN__
    while (csaci_g_looper_data.is_running && !csaci_window_should_close()) {
        csaci__looper_main_loop_wrapper();
    }
#else
    psaci_window_emscripten_set_loop_func(sb__looper_main_loop_wrapper, sb__looper_data.opts.desired_fps, SACI_TRUE);
#endif
}

void csaci_looper_stop(void)
{
    csaci_g_looper_data.is_running = SACI_FALSE;
#ifdef __EMSCRIPTEN__
    sb_window_emscripten_loop_cancel();
#endif
}

SaciBool csaci_looper_is_running(void)
{
    return csaci_g_looper_data.is_running;
}

// Internal funcs

SACI_INTERNAL CSaciLoopFrameData csaci__looper_get_frame_data(void)
{
    return (CSaciLoopFrameData){0};
}

SACI_INTERNAL void csaci__looper_main_loop_wrapper(void)
{
    csaci_g_looper_data.loop_func(csaci__looper_get_frame_data());
}
