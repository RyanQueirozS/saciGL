#ifndef __SACI_CORE_SC_RENDERING_H__
#define __SACI_CORE_SC_RENDERING_H__

#include "saci-utils/su-types.h"

//----------------------------------------------------------------------------//
// Render
//----------------------------------------------------------------------------//

typedef struct sc_Renderer sc_Renderer;

sc_Renderer* sc_CreateRenderer(void);
void sc_DeleteRenderer(sc_Renderer* renderer);

void sc_RenderBegin(sc_Renderer* renderer);
void sc_RenderEnd(sc_Renderer* renderer);

// Renderer config
void sc_RenderSetNoFillMode(void);
void sc_RenderSetFillMode(void);

// Triangles
void sc_RenderPushTriangle(sc_Renderer* renderer,
                               const saci_Vec2 a, const saci_Vec2 b, const saci_Vec2 c,
                               const saci_Color aColor, const saci_Color bColor, const saci_Color cColor);
#endif
