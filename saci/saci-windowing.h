#ifndef __SACI_SACI_WINDOWING_H__
#define __SACI_SACI_WINDOWING_H__

#include "saci-utils.h"
#include "sacigl-windowing.h"

typedef saciGL_Window saci_Window;
typedef saciGL_Monitor saci_Monitor;

void saci_InitWindow(int width, int height, const char* title);
void saci_Terminate(void);

saci_Bool saci_WindowShouldClose();

#endif
