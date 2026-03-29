#ifndef SACI_MAIN_SACI_H
#define SACI_MAIN_SACI_H

#include "saci_util/defines.h"

#include "saci_core/runtime/looper.h"
#include "saci_core/runtime/event.h"

typedef CSaciLoopFunc SaciLoopFunc;
typedef CSaciLoopFrameData SaciLoopFrameData;
typedef CSaciEvent SaciEvent;

typedef enum {
    SACI_RENDERING_FLAG_BLEND = 1U << 0,
} SaciFlags;

SACI_API void saci_init(void);

SACI_API void saci_enable(SaciFlags flag, SaciBool enable);

SACI_API void saci_begin(void);

SACI_API void saci_set_background_color(const SaciColor color);

SACI_API void saci_set_loop_func(SaciLoopFunc loop_func);

SACI_API const SaciEvent* saci_get_event(void);

SACI_API void saci_loop(void);

SACI_API void saci_present(void);

SACI_API void saci_end(void);

#endif // SACI_MAIN_SACI_H
