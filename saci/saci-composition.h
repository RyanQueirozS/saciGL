#ifndef __SACI_SACI_COMPOSITION_H__
#define __SACI_SACI_COMPOSITION_H__

#include "saci-shapes.h"
#include "saci-utils.h"

void saci_InitCompositor();

void saci_SetCanvasColor(const saci_Color color);

void saci_BeginComposition();
void saci_EndComposition();

// 2d
void saci_ComposeTriangle(const saciTri triangle, const saci_Color fillColor);
void saci_ComposeRect(const saciRect rect, const saci_Color fillColor);
void saci_ComposeQuad(const saciQuad quad, const saci_Color fillColor);

// 3d
void saci_ComposeCuboid(const saciCuboid cuboid, const saci_Color fillColor);

#endif
