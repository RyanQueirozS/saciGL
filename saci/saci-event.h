#ifndef __SACI_SACI_EVENT_H__
#define __SACI_SACI_EVENT_H__

#include "saci-utils.h"
#include "sacigl-event.h"

saci_Vec2 saci_GetMousePos();

typedef saciGL_Keycode saci_Keycode;

bool saci_IsKeyPressed(saci_Keycode keycode);

#endif
