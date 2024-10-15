#ifndef __SACI_CORE_SC_RENDERING_H__
#define __SACI_CORE_SC_RENDERING_H__

#include "saci-core/sc-camera.h"
#include "saci-utils/su-types.h"

//----------------------------------------------------------------------------//
// Render Initialization/Deletion
//----------------------------------------------------------------------------//

typedef struct sc_Renderer sc_Renderer;

sc_Renderer* sc_CreateRenderer(saci_Bool generateDefaults);
void sc_DeleteRenderer(sc_Renderer* renderer);

//----------------------------------------------------------------------------//
// Renderer config
//----------------------------------------------------------------------------//
void sc_RenderSetNoFillMode(void);
void sc_RenderSetFillMode(void);
void sc_RenderEnableZBuffer(void);

typedef enum sc_RenderProjectionMode {
    SACI_RENDER_ORTHOGRAPHIC_PROJECTION = 0,
    SACI_RENDER_PERSPECTIVE_PROJECTION = 1,
    SACI_RENDER_CUSTOM_PROJECTION = 2,
} sc_RendererProjectionMode;
void sc_RenderSetProjectionMode(sc_RendererProjectionMode renderProjectionMode);
typedef saci_Mat4 (*sc_RendererCustomProjectionFunction)(sc_Camera camera);
void sc_RenderSetCustomProjectionModeFunction(sc_RendererCustomProjectionFunction renderCustomProjectionMode);

//----------------------------------------------------------------------------//
// Renderer Usage
//----------------------------------------------------------------------------//

void sc_RenderBegin(sc_Renderer* renderer);
void sc_RenderEnd(sc_Renderer* renderer, const sc_Camera* camera);

void sc_RenderPushTriangleTexture(sc_Renderer* renderer,
                                  const saci_Vec3 a, const saci_Vec3 b, const saci_Vec3 c,
                                  const saci_Color aColor, const saci_Color bColor, const saci_Color cColor,
                                  const saci_Vec2 aUV, const saci_Vec2 bUV, const saci_Vec2 cUV,
                                  const saci_TextureID texID);
void sc_RenderPushTriangle2D(sc_Renderer* renderer,
                             const saci_Vec2 a, const saci_Vec2 b, const saci_Vec2 c, float depth,
                             const saci_Color aColor, const saci_Color bColor, const saci_Color cColor);

void sc_RenderPushTriangle3D(sc_Renderer* renderer,
                             const saci_Vec3 a, const saci_Vec3 b, const saci_Vec3 c,
                             const saci_Color aColor, const saci_Color bColor, const saci_Color cColor);
#endif
