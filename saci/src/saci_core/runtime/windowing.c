#include "saci_core/runtime/windowing.h"

#include "saci_util/defines.h"
#include "saci_platform/windowing/windowing.h"
#include "saci_util/types.h"
#include "saci_util/internal/log.h"
#include "saci_util/log.h"

struct CSaciWindowingCtx {
    SaciColor bg_color;
    union PSaciWindow window;
    SaciU8 id;
};

SACI_INTERNAL struct {
    struct CSaciWindowingCtx context_array[CSACI_WINDOWING_MAX_WINDOW_CONTEXTS];
    SaciU8 context_array_count;
} csaci_g_windowing_context;

void csaci_windowing_init(struct CSaciWindowProperties props, struct CSaciWindowingCtx* ctx_out)
{
    psaci_window_create(props.width, props.height, props.title, (union PSaciWindowOpts){0});
    if (!ctx_out) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_CRASH,
                         SACI_LOG_CONTEXT_WINDOWING_RUNTIME,
                         "Trying to init a windowing context with a null pointer");
    }
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
