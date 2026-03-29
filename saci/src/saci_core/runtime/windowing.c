#include "saci_core/runtime/windowing.h"

#include "saci_util/defines.h"
#include "saci_platform/windowing/windowing.h"
#include "saci_util/types.h"

struct CSaciWindowingCtx {
    SaciColor bg_color;
    union PSaciWindow window;
    SaciU8 id;
};

SACI_INTERNAL struct {
    struct CSaciWindowingCtx context_array[CSACI_WINDOWING_MAX_WINDOW_CONTEXTS];
    SaciU8 context_array_count;
    SaciU8 current_context;

    SaciBool is_loaded;
} csaci_g_windowing_context = {0};

void csaci_windowing_init(struct CSaciWindowProperties props, struct CSaciWindowingCtx* ctx_out)
{
    if (!csaci_g_windowing_context.is_loaded) {
        psaci_window_load();
    }
    SaciU8* ctx_count = &csaci_g_windowing_context.context_array_count;
    struct CSaciWindowingCtx* ctx =
        &csaci_g_windowing_context.context_array[*ctx_count];

    ctx->window = psaci_window_create(props.width, props.height, props.title,
                                      (union PSaciWindowOpts){0});

    csaci_g_windowing_context.current_context = (*ctx_count)++;
    ctx_out = ctx;
    ctx_out->window.glfw.height = props.height;
    ctx_out->window.glfw.width = props.width;
    ctx_out->window.glfw.x = props.x;
    ctx_out->window.glfw.y = props.y;
}

SaciU8 csaci_windowing_get_ctx_count(void)
{
    return csaci_g_windowing_context.context_array_count;
}

SaciBool csaci_window_should_close(void)
{
    SaciU8 current_context_idx = csaci_g_windowing_context.current_context;
    struct CSaciWindowingCtx context =
        csaci_g_windowing_context.context_array[current_context_idx];
    return psaci_window_should_close(context.window);
}
