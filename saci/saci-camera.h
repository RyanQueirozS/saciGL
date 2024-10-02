#ifndef __SACI_SACI_CAMERA_H__
#define __SACI_SACI_CAMERA_H__

#include "saci-utils.h"

typedef enum {
    SACI_CAMERA_MOVEMENT_XY = 0,        // Only movement in X and Y axes
    SACI_CAMERA_MOVEMENT_FREE_3D = 1,   // Full 3D movement and rotation
    SACI_CAMERA_MOVEMENT_LOCKED_3D = 2, // 3D movement without camera pos control
    SACI_CAMERA_FIXED_ANGLE_3D = 3,     // 3D with no angle adjustment
    SACI_CAMERA_FOCUS_TARGET = 4,       // Camera focuses on a central point
} saci_CameraMovementMode;

void saci_SetCameraMode(saci_CameraMovementMode cameraMode);

void saci_SetCameraPos(saci_Vec3 newPos);
void saci_SetCameraAngle(saci_Vec3 angleRad);
void saci_SetCameraTarget(saci_Vec3 lookingAtPos);
saci_Vec3 saci_GetCameraPos();
saci_Vec3 saci_GetCameraAngleRad();

#endif
