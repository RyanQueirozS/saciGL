#ifndef SACI_BACKEND_WINDOWING_SB_EMSDK_INTERNAL_H
#define SACI_BACKEND_WINDOWING_SB_EMSDK_INTERNAL_H

#include "saci-backend/windowing/sb-windowing-common.h"

#include "saci-utils/su-general.h"
#include "saci-utils/su-types-common.h"

SA_API union sb_Window sb_window_emscripten_create(int width, int height,
                                                   union sb_WindowOpts opts);

SA_API void sb_window_emscripten_free(union sb_Window window);

SA_API void sb_window_emscripten_make_context(union sb_Window window);

SA_API su_Bool sb_window_emscripten_should_close(union sb_Window window);

SA_API void sb_window_emscripten_set_pos_handler(union sb_Window window,
                                                 sb_WindowPosHandler handler);

SA_API void sb_window_emscripten_set_size_handler(union sb_Window window,
                                                  sb_WindowSizeHandler handler);

SA_API void sb_window_emscripten_set_loop_func(void* loop_func, int fps, su_Bool infinite_loop);

SA_API void sb_window_emscripten_loop_cancel(void);

SA_API void sb_event_emscripten_wait_for_timeout(double timeout);

SA_API void sb_event_emscripten_post_empty(void);

SA_API void sb_event_emscripten_set_mouse_pos_handler(union sb_Window window,
                                                      sb_WindowMousePosHandler handler);

SA_API su_Bool sb_event_emscripten_is_key_pressed(int keycode);

#endif // SACI_BACKEND_WINDOWING_SB_EMSDK_INTERNAL_H
