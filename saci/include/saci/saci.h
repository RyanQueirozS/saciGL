#ifndef SACI_MAIN_SACI_H
#define SACI_MAIN_SACI_H

#include "saci_util/defines.h"

#include "saci_core/runtime/looper.h"
#include "saci_core/runtime/event.h"

typedef CSaciMainLoopFunc SaciMainLoopFunc;
typedef CSaciLoopFrameData SaciLoopFrameData;
typedef CSaciEvent SaciEvent;

typedef enum {
    SACI_MODEL_FLAG_ROTATION_RTS = 1U << 0,
} SaciModelFlag;

typedef enum {
    SACI_RENDERING_FLAG_BLEND = 1U << 0,
} SaciFlags;

typedef struct SaciCube {
    SaciVec3 pos_center;
    SaciVec3 dimentions;
    SaciVec3 rotation;
    SaciColor fill_color;
    SaciModelFlag flags;
} SaciCube;

SACI_API void saci_init(void);

SACI_API void saci_enable(SaciFlags flag, SaciBool enable);

SACI_API void saci_begin(void);

SACI_API void saci_set_background_color(const SaciColor color);

SACI_API void saci_set_loop_func(SaciMainLoopFunc loop_func);

SACI_API const SaciEvent* saci_get_event(void);

SACI_API void saci_loop(void);

SACI_API void saci_draw_cube(const SaciCube cube);

SACI_API void saci_present(void);

SACI_API void saci_end(void);

#endif // SACI_MAIN_SACI_H
