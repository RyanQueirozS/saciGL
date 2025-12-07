#include "../internal/glfw.h"

SACI_INTERNAL struct PSaciWindowingApiFuncs psaci_g_windowing_api_funcs;

void psaci_window_glfw_load_dependencies(struct PSaciWindowingApiFuncs api_funcs)
{
    psaci_g_windowing_api_funcs = api_funcs;
}

SaciS32 psaci_window_glfw_init(void)
{
    return psaci_g_windowing_api_funcs.glfw.init();
}

void* psaci_window_glfw_get_proc(void)
{
    return (void*)psaci_g_windowing_api_funcs.glfw.get_proc;
}

union PSaciWindow psaci_window_glfw_create(int width, int height, const char* title,
                                           union PSaciWindowOpts opts)
{
    void* window = psaci_g_windowing_api_funcs.glfw.create_window(width, height, title, opts.glfw.monitor, opts.glfw.window_share);
    return (union PSaciWindow){
        .glfw = {
            .window = window,
            .width = width,
            .height = height,
            .title = title,
        },
    };
}

void psaci_window_glfw_free(union PSaciWindow window)
{
    psaci_g_windowing_api_funcs.glfw.destroy_window(window.glfw.window);
}

void psaci_window_glfw_make_context(union PSaciWindow window)
{
    psaci_g_windowing_api_funcs.glfw.make_context_current(window.glfw.window);
}

SaciBool psaci_window_glfw_should_close(union PSaciWindow window)
{
    return psaci_g_windowing_api_funcs.glfw.should_close(window.glfw.window);
}

void psaci_window_glfw_set_size_handler(union PSaciWindow window,
                                        SaciWindowSizeHandler window_size_handler)
{
    psaci_g_windowing_api_funcs.glfw.set_size_handler(window.glfw.window, window_size_handler);
}

void psaci_window_glfw_set_pos_handler(union PSaciWindow window,
                                       SaciWindowPosHandler window_pos_handler)
{
    psaci_g_windowing_api_funcs.glfw.set_pos_handler(window.glfw.window, window_pos_handler);
}

void psaci_window_glfw_terminate(void)
{
    psaci_g_windowing_api_funcs.glfw.terminate();
}

void psaci_window_glfw_swap_buffer(union PSaciWindow window)
{
    psaci_g_windowing_api_funcs.glfw.swap_buffers(window.glfw.window);
}

void psaci_event_glfw_poll(void)
{
    psaci_g_windowing_api_funcs.glfw.poll_events();
}

void psaci_event_glfw_wait(void)
{
    psaci_g_windowing_api_funcs.glfw.wait_events();
}

void psaci_event_glfw_wait_for_timeout(double timeout)
{
    psaci_g_windowing_api_funcs.glfw.wait_events_timeout(timeout);
}

void psaci_event_glfw_post_empty(void)
{
    psaci_g_windowing_api_funcs.glfw.post_empty_event();
}

void psaci_event_glfw_set_mouse_pos_handler(
    union PSaciWindow window, SaciEventMousePosHandler mouse_pos_handler)
{
    psaci_g_windowing_api_funcs.glfw.set_mouse_pos_handler(window.glfw.window, mouse_pos_handler);
}

SaciBool psaci_event_glfw_is_key_pressed(union PSaciWindow window, int keycode)
{
    return psaci_g_windowing_api_funcs.glfw.is_key_pressed(window.glfw.window, keycode);
}
