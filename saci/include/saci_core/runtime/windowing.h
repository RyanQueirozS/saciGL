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
    const char* title;
};

SACI_API void csaci_windowing_init(struct CSaciWindowProperties props, struct CSaciWindowingCtx* ctx_out);

SACI_API SaciU8 csaci_windowing_get_ctx_count(void);

SACI_API SaciBool csaci_window_should_close(void);

#endif // SACI_CORE_RUNTIME_WINDOWING_H
