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

void* sb_window_glfw_get_proc(void)
{
    return (void*)sb__windowing_api_funcs.glfw.get_proc;
}

union sb_Window sb_window_glfw_create(int width, int height, const char* title,
                                      union sb_WindowOpts opts)
{
    void* window = sb__windowing_api_funcs.glfw.create_window(width, height, title, opts.glfw.monitor, opts.glfw.window_share);
    return (union sb_Window){
        .glfw = {
            .window = window,
            .width = width,
            .height = height,
            .title = title,
        },
    };
}

void sb_window_glfw_free(union sb_Window window)
{
    sb__windowing_api_funcs.glfw.destroy_window(window.glfw.window);
}

void sb_window_glfw_make_context(union sb_Window window)
{
    sb__windowing_api_funcs.glfw.make_context_current(window.glfw.window);
}

su_Bool sb_window_glfw_should_close(union sb_Window window)
{
    return sb__windowing_api_funcs.glfw.should_close(window.glfw.window);
}

void sb_window_glfw_set_size_handler(union sb_Window window,
                                     su_WindowSizeHandler window_size_handler)
{
    sb__windowing_api_funcs.glfw.set_size_handler(window.glfw.window, window_size_handler);
}

void sb_window_glfw_set_pos_handler(union sb_Window window,
                                    su_WindowPosHandler window_pos_handler)
{
    sb__windowing_api_funcs.glfw.set_pos_handler(window.glfw.window, window_pos_handler);
}

void sb_window_glfw_terminate(void)
{
    sb__windowing_api_funcs.glfw.terminate();
}

void sb_window_glfw_swap_buffer(union sb_Window window)
{
    sb__windowing_api_funcs.glfw.swap_buffers(window.glfw.window);
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
    union sb_Window window, su_EventMousePosHandler mouse_pos_handler)
{
    sb__windowing_api_funcs.glfw.set_mouse_pos_handler(window.glfw.window, mouse_pos_handler);
}

su_Bool sb_event_glfw_is_key_pressed(union sb_Window window, int keycode)
{
    return sb__windowing_api_funcs.glfw.is_key_pressed(window.glfw.window, keycode);
}
