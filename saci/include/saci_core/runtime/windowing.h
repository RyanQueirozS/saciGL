#ifndef SACI_CORE_RUNTIME_WINDOWING_H
#define SACI_CORE_RUNTIME_WINDOWING_H

#include "saci_util/defines.h"
#include "saci_util/types.h"

// TODO find a reasonable number
#define CSACI_WINDOWING_MAX_WINDOW_CONTEXTS 8

struct CSaciWindowingCtx;

struct CSaciWindowProperties {
    int x, y;
    int width, height;
    SaciColor clear_color;
    const char* title;
};

SACI_API void csaci_windowing_init(struct CSaciWindowProperties props, struct CSaciWindowingCtx* ctx_out);

SACI_API SaciU8 csaci_windowing_get_ctx_count(void);

SACI_API SaciBool csaci_windowing_switch_ctx(SaciU8 idx);

SACI_API void csaci_windowing_set_clear_color(SaciColor color);

SACI_API void csaci_windowing_clear(void);

SACI_API SaciBool csaci_windowing_should_close(void);

SACI_API void csaci_windowing_present(void);

#endif // SACI_CORE_RUNTIME_WINDOWING_H
