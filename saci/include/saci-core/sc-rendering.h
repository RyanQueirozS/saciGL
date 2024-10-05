#ifndef __SACI_CORE_SC_RENDERING_H__
#define __SACI_CORE_SC_RENDERING_H__

#include "saci-utils/su-types.h"

//----------------------------------------------------------------------------//
// Render
//----------------------------------------------------------------------------//

typedef struct saciGL_Renderer saciGL_Renderer;

saciGL_Renderer* saciGL_CreateRenderer(void);
void saciGL_DeleteRenderer(saciGL_Renderer* renderer);

void saciGL_RenderBegin(saciGL_Renderer* renderer);
void saciGL_RenderEnd(saciGL_Renderer* renderer);

// Renderer config
void saciGL_RenderSetNoFillMode(void);
void saciGL_RenderSetFillMode(void);

// Triangles
void saciGL_RenderPushTriangle(saciGL_Renderer* renderer,
                               const saci_Vec2 a, const saci_Vec2 b, const saci_Vec2 c,
                               const saci_Color aColor, const saci_Color bColor, const saci_Color cColor);
#endif
