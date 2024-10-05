#ifndef __SACI_LIB_SL_EVENT_H__
#define __SACI_LIB_SL_EVENT_H__

#include "saci-core/sc-event.h"
#include "saci-utils/su-types.h"

saci_Vec2 saci_GetMousePos();

typedef saciGL_Keycode saci_Keycode;

saci_Bool saci_IsKeyPressed(saci_Keycode keycode);

#endif
