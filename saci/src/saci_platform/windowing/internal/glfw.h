#ifndef SACI_BACKEND_WINDOWING_SB_GLFW_INTERNAL_H
#define SACI_BACKEND_WINDOWING_SB_GLFW_INTERNAL_H

#include "saci_platform/windowing/windowing.h"

#include "saci_platform/dependencies/internal/dependency.h"
#include "saci_util/defines.h"
#include "saci_util/types.h"

#define PSACI_GLFW_CONTEXT_VERSION_MINOR 0x00022003
#define PSACI_GLFW_CONTEXT_VERSION_MAJOR 0x00022002
#define PSACI_GLFW_OPENGL_PROFILE 0x00022008
#define PSACI_GLFW_OPENGL_CORE_PROFILE 0x00032001
#define PSACI_GLFW_PRESS 1

SACI_API void psaci_window_glfw_load_dependencies(struct PSaciWindowingApiFuncs api_funcs);

SACI_API SaciS32 psaci_window_glfw_init(void);

SACI_API void* psaci_window_glfw_get_proc(void);

SACI_API union PSaciWindow psaci_window_glfw_create(int width, int height, const char* title,
                                                    union PSaciWindowOpts);

SACI_API void psaci_window_glfw_free(union PSaciWindow window);

SACI_API void psaci_window_glfw_make_context(union PSaciWindow window);

SACI_API SaciBool psaci_window_glfw_should_close(union PSaciWindow window);

SACI_API void psaci_window_glfw_set_size_handler(union PSaciWindow window,
                                                 SaciWindowSizeHandler window_size_handler);

SACI_API void psaci_window_glfw_set_pos_handler(union PSaciWindow window,
                                                SaciWindowPosHandler window_pos_handler);

SACI_API void psaci_window_glfw_terminate(void);

SACI_API void psaci_window_glfw_swap_buffer(union PSaciWindow window);

SACI_API void psaci_event_glfw_poll(void);

SACI_API void psaci_event_glfw_wait(void);

SACI_API void psaci_event_glfw_wait_for_timeout(double timeout);

SACI_API void psaci_event_glfw_post_empty(void);

SACI_API void psaci_event_glfw_set_mouse_pos_handler(
    union PSaciWindow window, SaciEventMousePosHandler mouse_pos_handler);

SACI_API SaciBool psaci_event_glfw_is_key_pressed(union PSaciWindow window, int keycode);

#endif // SACI_BACKEND_WINDOWING_SB_GLFW_INTERNAL_H
