#ifndef __SACI_LIB_SL_SHAPES_H__
#define __SACI_LIB_SL_SHAPES_H__

#include "saci-utils/su-types.h"

// 2d
typedef struct saciRect {
    saci_Vec2 pos;

    float width, height;
} saciRect;

typedef struct saciTri {
    saci_Vec2 a, b, c;
} saciTri;

typedef struct saciQuad {
    saci_Vec2 a, b, c, d;
} saciQuad;

// 3d // TODO: TO BE IMPLEMENTED
typedef struct saciCuboid {
    saci_Vec3 pos;

    float width, height, length;
} saciCuboid;

#endif
