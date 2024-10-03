#ifndef __SACI_SACI_CAMERA_H__
#define __SACI_SACI_CAMERA_H__

#include "saci-utils.h"

typedef enum {
    SACI_CAMERA_MOVEMENT_XY = 0,      // Only movement in X and Y axes
    SACI_CAMERA_MOVEMENT_FREE_3D = 1, // Full 3D movement and rotation
    SACI_CAMERA_MOVEMENT_XY_ZOOM = 2, // 2D movement without camera zoom
    SACI_CAMERA_FOCUS_TARGET = 3,     // Camera focuses on a central point
} saci_CameraMovementMode;

void saci_SetCameraMode(saci_CameraMovementMode cameraMode);

void saci_SetCameraPos(saci_Vec3 newPos);
void saci_SetCameraTarget(saci_Vec3 lookingAtPos);
void saci_SetCameraUp(saci_Vec3 up);
saci_Vec3 saci_GetCameraPos(void);
saci_Vec3 saci_GetCameraUp(void);

#endif
