#ifndef SACI_BACKEND_WINDOWING_SB_GLFW_INTERNAL_H
#define SACI_BACKEND_WINDOWING_SB_GLFW_INTERNAL_H

#include "saci-backend/windowing/sb-windowing.h"

#include "saci-backend/resources/sb-dependency-internal.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-types-common.h"

#define sb_GLFW_CONTEXT_VERSION_MINOR 0x00022003
#define sb_GLFW_CONTEXT_VERSION_MAJOR 0x00022002
#define sb_GLFW_OPENGL_PROFILE 0x00022008
#define sb_GLFW_OPENGL_CORE_PROFILE 0x00032001
#define sb_GLFW_PRESS 1

SA_API void sb_window_glfw_load_dependencies(struct sb_WindowingApiFuncs api_funcs);

SA_API su_S32 sb_window_glfw_init(void);

SA_API void* sb_window_glfw_get_proc(void);

SA_API union sb_Window sb_window_glfw_create(int width, int height, const char* title,
                                             union sb_WindowOpts);

SA_API void sb_window_glfw_free(union sb_Window window);

SA_API void sb_window_glfw_make_context(union sb_Window window);

SA_API su_Bool sb_window_glfw_should_close(union sb_Window window);

SA_API void sb_window_glfw_set_size_handler(union sb_Window window,
                                            su_WindowSizeHandler window_size_handler);

SA_API void sb_window_glfw_set_pos_handler(union sb_Window window,
                                           su_WindowPosHandler window_pos_handler);

SA_API void sb_window_glfw_terminate(void);

SA_API void sb_window_glfw_swap_buffer(union sb_Window window);

SA_API void sb_event_glfw_poll(void);

SA_API void sb_event_glfw_wait(void);

SA_API void sb_event_glfw_wait_for_timeout(double timeout);

SA_API void sb_event_glfw_post_empty(void);

SA_API void sb_event_glfw_set_mouse_pos_handler(
    union sb_Window window, su_EventMousePosHandler mouse_pos_handler);

SA_API su_Bool sb_event_glfw_is_key_pressed(union sb_Window window, int keycode);

#endif // SACI_BACKEND_WINDOWING_SB_GLFW_INTERNAL_H
