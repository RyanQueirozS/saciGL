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

typedef scglWindow saciWindow;
typedef scglMonitor saciMonitor;

void saciInitWindow(int width, int height, const char* title);
void saciTerminate(void);

bool saciWindowShouldClose();

//----------------------------------------------------------------------------//
// Composition
//----------------------------------------------------------------------------//

void saciInitCompositor();

void saciSetCanvasColor(const Color color);

void saciBeginComposition();
void saciEndComposition();

// 2d
void saciComposeTriangle(const saciTri triangle, const Color fillColor);
void saciComposeRect(const saciRect rect, const Color fillColor);
void saciComposeQuad(const saciQuad quad, const Color fillColor);

// 3d
void saciComposeCuboid(const saciCuboid cuboid, const Color fillColor);

//----------------------------------------------------------------------------//
// Camera
//----------------------------------------------------------------------------//

typedef enum {
    SACI_CAMERA_MOVEMENT_XY = 0,        // Only movement in X and Y axes
    SACI_CAMERA_MOVEMENT_FREE_3D = 1,   // Full 3D movement and rotation
    SACI_CAMERA_MOVEMENT_LOCKED_3D = 2, // 3D movement without camera pos control
    SACI_CAMERA_FIXED_ANGLE_3D = 3,     // 3D with no angle adjustment
    SACI_CAMERA_FOCUS_TARGET = 4,       // Camera focuses on a central point
} saciCameraMovementMode;

void saciSetCameraMode(saciCameraMovementMode cameraMode);

void saciSetCameraPos(Vec3 newPos);
void saciSetCameraAngle(Vec3 angleRad);
void saciSetCameraTarget(Vec3 lookingAtPos);
Vec3 saciGetCameraPos();
Vec3 saciGetCameraAngleRad();

//----------------------------------------------------------------------------//
// Event
//----------------------------------------------------------------------------//

Vec2 saciGetMousePos();

typedef scglKeycode saciKeycode;

bool saciIsKeyPressed(saciKeycode keycode);

#endif
