#ifndef __SACI_SHAPES_H__
#define __SACI_SHAPES_H__

#include "helpers.h"

typedef struct saciRect {
    float x, y;

    float width, height;
} saciRect;

typedef struct saciTriangle {
    vec2 a, b, c;
} saciTriangle;

#endif
