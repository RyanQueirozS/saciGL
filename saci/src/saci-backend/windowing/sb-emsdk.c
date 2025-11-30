#include "./sb-emsdk-internal.h"

#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include "saci-utils/su-log.h"

SA_INTERNAL su_Bool sb__emscripten_context_lost(int eventType, const void* event, void* user_data);
SA_INTERNAL su_Bool sb__emscripten_context_restored(int eventType, const void* event, void* user_data);
SA_INTERNAL su_Bool sb__emscripten_window_resized(int eventType, const EmscriptenUiEvent* event, void* user_data);
SA_INTERNAL su_Bool sb__emscripten_mouse_move(int eventType, const EmscriptenMouseEvent* event, void* user_data);
SA_INTERNAL void sb__emscripten_empty_callback(void* user_data);

SA_STATIC EmscriptenWebGLContextAttributes sb__emscripten_default_attrs = {
    .alpha = su_TRUE,
    .depth = su_TRUE,
    .stencil = su_FALSE,
    .antialias = su_TRUE,
    .majorVersion = 2, // WebGL 2.0 (OpenGL ES 3.0)
    .minorVersion = 0,
};

union sb_Window sb_window_emscripten_create(int width, int height,
                                            union sb_WindowOpts opts)
{
    union sb_Window window = {0};

    // Set up context attributes
    EmscriptenWebGLContextAttributes attrs = sb__emscripten_default_attrs;

    // Apply custom options if provided
    if (opts.emscripten.major_version) {
        attrs.majorVersion = opts.emscripten.major_version;
    }
    if (opts.emscripten.minor_version) {
        attrs.minorVersion = opts.emscripten.minor_version;
    }

    // Create WebGL context
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context =
        emscripten_webgl_create_context("#canvas", &attrs);

    if (context <= 0) {
        su_LOG_ERRORF_M(su_LOG_TYPE_PROD, su_LOG_ERROR_SEVERITY_CRASH,
                        su_LOG_CONTEXT_WINDOWING, "Failed to create WebGL context: %lu", context);
        return window;
    }

    window.emscripten.context = context;
    window.emscripten.selector = "#canvas";
    window.emscripten.width = width;
    window.emscripten.height = height;
    window.emscripten.target_width = width;
    window.emscripten.target_height = height;
    window.emscripten.context_lost = su_FALSE;

    // Set context loss callbacks
    emscripten_set_webglcontextlost_callback("#canvas", &window, 0,
                                             sb__emscripten_context_lost);
    emscripten_set_webglcontextrestored_callback("#canvas", &window, 0,
                                                 sb__emscripten_context_restored);

    return window;
}

void sb_window_emscripten_free(union sb_Window window)
{
    if (window.emscripten.context > 0) {
        emscripten_webgl_destroy_context(window.emscripten.context);
    }
}

void sb_window_emscripten_make_context(union sb_Window window)
{
    emscripten_webgl_make_context_current(window.emscripten.context);
}

su_Bool sb_window_emscripten_should_close(union sb_Window window)
{
    // Emscripten windows don't "close" in the traditional sense
    // Return true if context was lost and not restored
    return window.emscripten.context_lost;
}

void sb_window_emscripten_set_size_handler(union sb_Window window,
                                           sb_WindowSizeHandler handler)
{
    window.emscripten.size_handler = handler;

    // Set up resize callback for browser window
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, &window, 0,
                                   sb__emscripten_window_resized);
}

void sb_event_emscripten_wait_for_timeout(double timeout)
{
    // Not applicable in the same way
    emscripten_sleep(timeout * 1000); // Convert to milliseconds
}

void sb_event_emscripten_post_empty(void)
{
    // Can simulate with a simple async call
    emscripten_async_call(sb__emscripten_empty_callback, NULL, 0);
}

void sb_event_emscripten_set_mouse_pos_handler(union sb_Window window,
                                               sb_WindowMousePosHandler handler)
{
    window.emscripten.mouse_pos_handler = handler;

    emscripten_set_mousemove_callback(window.emscripten.selector, &window, 0,
                                      sb__emscripten_mouse_move);
}

su_Bool sb_event_emscripten_is_key_pressed(int keycode)
{
    // TODO
    // Use Emscripten's key state system
    EM_ASM_INT({ return Module['keyStates'] && Module['keyStates'][$0] ? 1 : 0; }, keycode);
}

// Internal callback functions
su_Bool sb__emscripten_context_lost(int eventType, const void* event, void* user_data)
{
    (void)eventType, (void)event;
    union sb_Window* window = (union sb_Window*)user_data;
    window->emscripten.context_lost = su_TRUE;
    su_LOG_ERROR_M(su_LOG_TYPE_PROD, su_LOG_ERROR_SEVERITY_HIGH,
                   su_LOG_CONTEXT_WINDOWING, "WebGL context lost");
    return EM_TRUE;
}

su_Bool sb__emscripten_context_restored(int eventType, const void* event, void* user_data)
{
    (void)eventType, (void)event;
    union sb_Window* window = (union sb_Window*)user_data;
    window->emscripten.context_lost = su_FALSE;
    su_LOG_INFO_M(su_LOG_TYPE_PROD, su_LOG_CONTEXT_WINDOWING, "WebGL context restored");
    return EM_TRUE;
}

su_Bool sb__emscripten_window_resized(int eventType, const EmscriptenUiEvent* event, void* user_data)
{
    (void)eventType, (void)event;
    union sb_Window* window = (union sb_Window*)user_data;

    if (window->emscripten.size_handler) {
        // Get actual canvas size
        double width, height;
        emscripten_get_element_css_size(window->emscripten.selector, &width, &height);

        window->emscripten.width = (int)width;
        window->emscripten.height = (int)height;

        window->emscripten.size_handler(window, (int)width, (int)height);
    }
    return EM_TRUE;
}

su_Bool sb__emscripten_mouse_move(int eventType, const EmscriptenMouseEvent* event, void* user_data)
{
    (void)eventType, (void)event;
    union sb_Window* window = (union sb_Window*)user_data;

    if (window->emscripten.mouse_pos_handler) {
        window->emscripten.mouse_pos_handler(window, event->clientX, event->clientY);
    }
    return EM_TRUE;
}

void sb__emscripten_empty_callback(void* user_data)
{
    (void)user_data;
}
