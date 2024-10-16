#ifndef __SACI_CORE_SC_RENDERING_H__
#define __SACI_CORE_SC_RENDERING_H__

#include "saci-core/sc-camera.h"
#include "saci-utils/su-types.h"

/* === Render Struct === */

/**
 * @struct sc_Renderer
 * @brief Structure to hold render related information.
 *
 * @details
 * This structure is used to hold vertex information in form of triangles, squares or lines.
 * These shapes are later rendered to the screen.
 */
typedef struct sc_Renderer sc_Renderer;

/* === Render Initialization === */

/**
 * @brief Creates the sc_Renderer struct
 *
 * @details
 * This functions creates, initializes and sets all of the sc_Renderer related shaders and OpenGL context.
 *
 * @param generateDefaults A boolean to generate defaulted shaders and OpenGL context.
 * @return A new sc_Renderer* either defaulted or not. Does check if it's null
 */
sc_Renderer* sc_CreateRenderer(saci_Bool generateDefaults);

/**
 * @brief Deletes the sc_Renderer struct
 *
 * @details
 * This functions deletes a sc_Renderer
 *
 * @param renderer The renderer to be deleted
 */
void sc_DeleteRenderer(sc_Renderer* renderer);

/* === Render Config === */

/**
 * @brief Sets renderer to not fill shapes
 */
void sc_RenderSetNoFillMode(void);

/**
 * @brief Sets renderer to fill shapes
 * @note This is the defaulted option.
 */
void sc_RenderSetFillMode(void);

/**
 * @brief Enables the Z buffer.
 *
 * @details
 * Check OpenGL article: https://learnopengl.com/Advanced-OpenGL/Depth-testing
 */
void sc_RenderEnableZBuffer(void);

/**
 * @enum sc_RenderProjectionType
 * @brief Types of Projection that a sc_Renderer can use.
 */
typedef enum sc_RenderProjectionMode {
    SACI_RENDER_ORTHOGRAPHIC_PROJECTION = 0,
    SACI_RENDER_PERSPECTIVE_PROJECTION = 1,
    SACI_RENDER_CUSTOM_PROJECTION = 2,
} sc_RendererProjectionMode;

/**
 * @brief Sets projection mode
 *
 * @param renderProjectionMode The sc_RenderProjectionMode that the sc_Renderer should use.
 */
void sc_RenderSetProjectionMode(sc_RendererProjectionMode renderProjectionMode);

/**
 * @typedef sc_RenderCustomProjectionFunction
 * @brief A function pointer that's used for custom projections
 *
 * @param camera The sc_Camera struct used to setup projection
 * @return A saci_Mat4 with the projection values
 */
typedef saci_Mat4 (*sc_RendererCustomProjectionFunction)(sc_Camera camera);

/**
 * @brief Sets custom projection mode function.
 *
 * @param renderCustomProjectionModeFunction The sc_RenderProjectionMode that the sc_Renderer should use.
 */
void sc_RenderSetCustomProjectionModeFunction(sc_RendererCustomProjectionFunction renderCustomProjectionModeFunction);

/* === Render Usage === */

/**
 * @brief Sets renderer to begins rendering the frame
 *
 * @param renderer The renderer to setup
 */
void sc_RenderBegin(sc_Renderer* renderer);

/**
 * @brief Draws the RenderCalls in the sc_Renderer
 *
 * @details
 * Each sc_Renderer has a non set size array of RenderCalls called RenderBatch,
 * RenderCalls store the vertice information, as well as the renderingMode(triangle,
 * quad, line), and the texture that will be used (if any).
 * When called, this function Draws everything stored in the this RenderBatch
 * using the VAO,VBO and shader program in sc_Renderer.
 *
 * @param renderer The renderer to setup
 */
void sc_RenderEnd(sc_Renderer* renderer, const sc_Camera* camera);

/**
 * @brief Pushes a textured 3D plane triangle to the renderer.
 *
 * @param renderer The target renderer.
 * @param a, b, c The triangle vertices.
 * @param aColor, bColor, cColor The colors for each vertex.
 * @param aUV, bUV, cUV The UV coordinates for each vertex.
 * @param texID The texture to apply.
 */
void sc_RenderPushTriangleTexture(sc_Renderer* renderer,
                                  const saci_Vec3 a, const saci_Vec3 b, const saci_Vec3 c,
                                  const saci_Color aColor, const saci_Color bColor, const saci_Color cColor,
                                  const saci_Vec2 aUV, const saci_Vec2 bUV, const saci_Vec2 cUV,
                                  const saci_TextureID texID);

/**
 * @brief Pushes a textured 2D plane triangle to the renderer.
 *
 * @param renderer The target renderer.
 * @param a, b, c The triangle vertices.
 * @param aColor, bColor, cColor The colors for each vertex.
 */
void sc_RenderPushTriangle2D(sc_Renderer* renderer,
                             const saci_Vec2 a, const saci_Vec2 b, const saci_Vec2 c, float depth,
                             const saci_Color aColor, const saci_Color bColor, const saci_Color cColor);

/**
 * @brief Pushes a textured 3D plane triangle to the renderer.
 *
 * @param renderer The target renderer.
 * @param a, b, c The triangle vertices.
 * @param aColor, bColor, cColor The colors for each vertex.
 */
void sc_RenderPushTriangle3D(sc_Renderer* renderer,
                             const saci_Vec3 a, const saci_Vec3 b, const saci_Vec3 c,
                             const saci_Color aColor, const saci_Color bColor, const saci_Color cColor);
#endif
