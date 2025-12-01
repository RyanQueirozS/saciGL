#include "saci-backend/windowing/sb-looper.h"

#include "./sb-emsdk-internal.h"

// Internal funcs

SA_INTERNAL sb_MainLoopData sb__looper_get_frame_data(void);

SA_INTERNAL void sb__looper_main_loop_wrapper(void);

// Header impl

SA_INTERNAL struct {
    sb_MainLoopFunc loop_func;
    sb_MainLoopOpts opts;
    su_Bool is_running;
} sb__looper_data = {0};

void sb_looper_set_main_loop(sb_MainLoopFunc loop_func, sb_MainLoopOpts loop_opts)
{
    sb__looper_data.loop_func = loop_func;
    sb__looper_data.opts = loop_opts;
}

void sb_looper_run(void)
{
    if (sb__looper_data.is_running) {
        return;
    }

    sb__looper_data.is_running = su_TRUE;
#ifndef __EMSCRIPTEN__
    while (sb__looper_data.is_running) {
        sb__looper_main_loop_wrapper();
    }
#else
    sb_window_emscripten_set_loop_func(sb__looper_main_loop_wrapper, sb__looper_data.opts.desired_fps, su_TRUE);
#endif
}

void sb_looper_stop(void)
{
    sb__looper_data.is_running = su_FALSE;
#ifdef __EMSCRIPTEN__
    sb_window_emscripten_loop_cancel();
#endif
}

su_Bool sb_looper_is_running(void)
{
    return sb__looper_data.is_running;
}

// Internal funcs

SA_INTERNAL sb_MainLoopData sb__looper_get_frame_data(void)
{
    return (sb_MainLoopData){0};
}

SA_INTERNAL void sb__looper_main_loop_wrapper(void)
{
    sb__looper_data.loop_func(sb__looper_get_frame_data());
}
