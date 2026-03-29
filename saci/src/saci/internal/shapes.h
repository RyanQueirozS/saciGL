#ifndef SACI_INTERNAL_SHAPES_H
#define SACI_INTERNAL_SHAPES_H

#include "saci_util/defines.h"
#include "saci_util/types.h"

SACI_STATIC const SaciVec3 SACI_G_CUBE_POS[8] = {
    {-1, -1, -1},
    {-1, -1, 1},
    {-1, 1, -1},
    {-1, 1, 1},
    {1, -1, -1},
    {1, -1, 1},
    {1, 1, -1},
    {1, 1, 1},
};

SACI_STATIC const SaciS32 SACI_G_CUBE_IDX[36] = {
    1, 5, 7, 1, 7, 3,
    0, 2, 6, 0, 6, 4,
    0, 1, 3, 0, 3, 2,
    4, 6, 7, 4, 7, 5,
    2, 3, 7, 2, 7, 6,
    0, 4, 5, 0, 5, 1};

#endif // SACI_INTERNAL_SHAPES_H
