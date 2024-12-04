/**
 * @file sc-rendering.h
 * @brief This header defines camera related functions and structs related to saciCORE
 */
#ifndef __SACI_CORE_SC_CAMERA_H__
#define __SACI_CORE_SC_CAMERA_H__

#include "saci-utils/su-types.h"

typedef struct sc_Camera {
    saci_Vec3 position;
    saci_Vec3 target;
    saci_Vec3 up;

    float     fov;
    float     aspectRatio;
    float     near;
    float     far;
} sc_Camera;

sc_Camera sc_Camera_Get_Default(void);

#endif
