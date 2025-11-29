#include "./sb-glfw-internal.h"

SA_INTERNAL struct sb_WindowingApiFuncs sb__windowing_api_funcs;

void sb_window_glfw_load_dependencies(struct sb_WindowingApiFuncs api_funcs)
{
    sb__windowing_api_funcs = api_funcs;
}

su_S32 sb_window_glfw_init(void)
{
    return sb__windowing_api_funcs.glfw.init();
}

void* sb_window_glfw_get_proc(const char*) {}

su_Window sb_window_glfw_create(int width, int height, const char* title,
                                su_Monitor monitor, su_Window share)
{
    return sb__windowing_api_funcs.glfw.create_window(width, height, title, monitor, share);
}

void sb_window_glfw_free(su_Window window)
{
    sb__windowing_api_funcs.glfw.destroy_window(window);
}

void sb_window_glfw_make_context(su_Window window)
{
    sb__windowing_api_funcs.glfw.make_context_current(window);
}

su_Bool sb_window_glfw_should_close(su_Window window)
{
    return sb__windowing_api_funcs.glfw.should_close(window);
}

void sb_window_glfw_set_size_handler(su_Window window,
                                     su_WindowSizeHandler window_size_handler)
{
    sb__windowing_api_funcs.glfw.set_size_handler(window, window_size_handler);
}

void sb_window_glfw_set_pos_handler(su_Window window,
                                    su_WindowPosHandler window_pos_handler)
{
    sb__windowing_api_funcs.glfw.set_pos_handler(window, window_pos_handler);
}

void sb_window_glfw_terminate(void)
{
    sb__windowing_api_funcs.glfw.terminate();
}

void sb_window_glfw_swap_buffer(su_Window window)
{
    sb__windowing_api_funcs.glfw.swap_buffers(window);
}

void sb_event_glfw_poll(void)
{
    sb__windowing_api_funcs.glfw.poll_events();
}

void sb_event_glfw_wait(void)
{
    sb__windowing_api_funcs.glfw.wait_events();
}

void sb_event_glfw_wait_for_timeout(double timeout)
{
    sb__windowing_api_funcs.glfw.wait_events_timeout(timeout);
}

void sb_event_glfw_post_empty(void)
{
    sb__windowing_api_funcs.glfw.post_empty_event();
}

void sb_event_glfw_set_mouse_pos_handler(
    su_Window window, su_EventMousePosHandler mouse_pos_handler)
{
    sb__windowing_api_funcs.glfw.set_mouse_pos_handler(window, mouse_pos_handler);
}

su_Bool sb_event_glfw_is_key_pressed(su_Window window, int keycode)
{
    return sb__windowing_api_funcs.glfw.is_key_pressed(window, keycode);
}
