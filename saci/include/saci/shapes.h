#ifndef SACI_SHAPES_H
#define SACI_SHAPES_H

#include "saci_util/defines.h"
#include "saci_util/types.h"

typedef enum {
    SACI_MODEL_FLAG_ROTATION_RTS = 1U << 0,
} SaciModelFlag;

typedef struct SaciCube {
    SaciVec3 pos_center;
    SaciVec3 dimentions;
    SaciVec3 rotation;
    SaciColor fill_color;
    SaciModelFlag flags;
} SaciCube;

SACI_API void saci_draw_cube(const SaciCube cube);

#endif // SACI_SHAPES_H
