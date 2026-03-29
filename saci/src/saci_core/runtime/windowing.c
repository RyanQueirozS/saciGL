#include "saci_core/runtime/windowing.h"

#include "saci_util/defines.h"
#include "saci_platform/windowing/windowing.h"
#include "saci_util/types.h"
#include <saci_platform/gfx/graphics.h>

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

    psaci_window_make_context(ctx->window);
    csaci_g_windowing_context.current_context = (*ctx_count)++;
    ctx_out = ctx;
    ctx_out->window.glfw.height = props.height;
    ctx_out->window.glfw.width = props.width;
    ctx_out->window.glfw.x = props.x;
    ctx_out->window.glfw.y = props.y;
    ctx_out->bg_color = props.clear_color;
}

SaciU8 csaci_windowing_get_ctx_count(void)
{
    return csaci_g_windowing_context.context_array_count;
}

SaciBool csaci_windowing_switch_ctx(SaciU8 idx)
{
    if (csaci_g_windowing_context.context_array_count <= idx) {
        return SACI_FALSE;
    }
    csaci_g_windowing_context.current_context = idx;
    return SACI_TRUE;
}

void csaci_windowing_set_clear_color(SaciColor color)
{
    SaciU8 current_context_idx = csaci_g_windowing_context.current_context;
    struct CSaciWindowingCtx* context_ptr =
        &csaci_g_windowing_context.context_array[current_context_idx];
    context_ptr->bg_color = color;
}

void csaci_windowing_clear(void)
{
    SaciU8 current_context_idx = csaci_g_windowing_context.current_context;
    struct CSaciWindowingCtx context =
        csaci_g_windowing_context.context_array[current_context_idx];

    psaci_graphics_clear_color(context.bg_color);
}

SaciBool csaci_windowing_should_close(void)
{
    SaciU8 current_context_idx = csaci_g_windowing_context.current_context;
    struct CSaciWindowingCtx context =
        csaci_g_windowing_context.context_array[current_context_idx];
    return psaci_window_should_close(context.window);
}

void csaci_windowing_present(void)
{
    SaciU8 current_context_idx = csaci_g_windowing_context.current_context;
    struct CSaciWindowingCtx context =
        csaci_g_windowing_context.context_array[current_context_idx];
    psaci_window_swap_buffer(context.window);
}
