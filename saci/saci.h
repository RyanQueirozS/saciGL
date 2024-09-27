#ifndef __SACI_SACI_H__
#define __SACI_SACI_H__

#include <stdint.h>

#include "gl.h"
#include "utils.h"
#include "shapes.h"

//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Windowing
//----------------------------------------------------------------------------//

typedef saciGL_Window saci_Window;
typedef saciGL_Monitor saci_Monitor;

void saci_InitWindow(int width, int height, const char* title);
void saci_Terminate(void);

bool saci_WindowShouldClose();

//----------------------------------------------------------------------------//
// Composition
//----------------------------------------------------------------------------//

void saci_InitCompositor();

void saci_SetCanvasColor(const Color color);

void saci_BeginComposition();
void saci_EndComposition();

// 2d
void saci_ComposeTriangle(const saciTri triangle, const Color fillColor);
void saci_ComposeRect(const saciRect rect, const Color fillColor);
void saci_ComposeQuad(const saciQuad quad, const Color fillColor);

// 3d
void saci_ComposeCuboid(const saciCuboid cuboid, const Color fillColor);

//----------------------------------------------------------------------------//
// Camera
//----------------------------------------------------------------------------//

typedef enum {
    SACI_CAMERA_MOVEMENT_XY = 0,        // Only movement in X and Y axes
    SACI_CAMERA_MOVEMENT_FREE_3D = 1,   // Full 3D movement and rotation
    SACI_CAMERA_MOVEMENT_LOCKED_3D = 2, // 3D movement without camera pos control
    SACI_CAMERA_FIXED_ANGLE_3D = 3,     // 3D with no angle adjustment
    SACI_CAMERA_FOCUS_TARGET = 4,       // Camera focuses on a central point
} saci_CameraMovementMode;

void saci_SetCameraMode(saci_CameraMovementMode cameraMode);

void saci_SetCameraPos(Vec3 newPos);
void saci_SetCameraAngle(Vec3 angleRad);
void saci_SetCameraTarget(Vec3 lookingAtPos);
Vec3 saci_GetCameraPos();
Vec3 saci_GetCameraAngleRad();

//----------------------------------------------------------------------------//
// Event
//----------------------------------------------------------------------------//

Vec2 saci_GetMousePos();

typedef saciGL_Keycode saci_Keycode;

bool saci_IsKeyPressed(saci_Keycode keycode);

#endif
