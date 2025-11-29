#ifndef SACI_BACKEND_WINDOWING_SB_GLFW_INTERNAL_H
#define SACI_BACKEND_WINDOWING_SB_GLFW_INTERNAL_H

#include "saci-utils/su-general.h"
#include "saci-utils/su-types-common.h"
#include "saci-backend/resources/sb-dependency-internal.h"

#define sb_GLFW_CONTEXT_VERSION_MINOR 0x00022003
#define sb_GLFW_CONTEXT_VERSION_MAJOR 0x00022002
#define sb_GLFW_OPENGL_PROFILE 0x00022008
#define sb_GLFW_OPENGL_CORE_PROFILE 0x00032001
#define sb_GLFW_PRESS 1

SA_API void sb_window_glfw_load_dependencies(struct sb_WindowingApiFuncs api_funcs);

SA_API su_S32 sb_window_glfw_init(void);

SA_API void* sb_window_glfw_get_proc(const char*);

SA_API su_Window sb_window_glfw_create(int width, int height, const char* title,
                                       su_Monitor monitor, su_Window share);

SA_API void sb_window_glfw_free(su_Window window);

SA_API void sb_window_glfw_make_context(su_Window window);

SA_API su_Bool sb_window_glfw_should_close(su_Window window);

SA_API void sb_window_glfw_set_size_handler(su_Window window,
                                            su_WindowSizeHandler window_size_handler);

SA_API void sb_window_glfw_set_pos_handler(su_Window window,
                                           su_WindowPosHandler window_pos_handler);

SA_API void sb_window_glfw_terminate(void);

SA_API void sb_window_glfw_swap_buffer(su_Window window);

SA_API void sb_event_glfw_poll(void);

SA_API void sb_event_glfw_wait(void);

SA_API void sb_event_glfw_wait_for_timeout(double timeout);

SA_API void sb_event_glfw_post_empty(void);

SA_API void sb_event_glfw_set_mouse_pos_handler(
    su_Window window, su_EventMousePosHandler mouse_pos_handler);

SA_API su_Bool sb_event_glfw_is_key_pressed(su_Window window, int keycode);

#endif // SACI_BACKEND_WINDOWING_SB_GLFW_INTERNAL_H
