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
    SaciBool is_loaded;
} csaci_g_windowing_context = {0};

void csaci_windowing_init(struct CSaciWindowProperties props, struct CSaciWindowingCtx* ctx_out)
{
    if (!csaci_g_windowing_context.is_loaded) {
        psaci_window_load();
    }
    psaci_window_create(props.width, props.height, props.title, (union PSaciWindowOpts){0});
    struct CSaciWindowingCtx* new_ctx = &csaci_g_windowing_context.context_array[++csaci_g_windowing_context.context_array_count];
    new_ctx->window.glfw.height = props.height;
    new_ctx->window.glfw.width = props.width;
    new_ctx->window.glfw.x = props.x;
    new_ctx->window.glfw.y = props.y;
    ctx_out = new_ctx;
}

SaciU8 csaci_windowing_get_ctx_count(void)
{
    return csaci_g_windowing_context.context_array_count;
}
