#ifndef __SACI_SHAPES_H__
#define __SACI_SHAPES_H__

#include "utils.h"

// 2d
typedef struct saciRect {
    Vec2 pos;

    float width, height;
} saciRect;

typedef struct saciTri {
    Vec2 a, b, c;
} saciTri;

typedef struct saciQuad {
    Vec2 a, b, c, d;
} saciQuad;

// 3d // TODO: TO BE IMPLEMENTED
typedef struct saciCuboid {
    Vec3 pos;

    float width, height, length;
} saciCuboid;

#endif
