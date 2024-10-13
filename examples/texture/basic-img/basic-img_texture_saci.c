#include "saci-core/sc-camera.h"
#include "saci-core/sc-event.h"
#include "saci-core/sc-rendering.h"
#include "saci-core/sc-texture.h"
#include "saci-core/sc-windowing.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"

#include <assert.h>
#include <stdio.h>

sc_Window* window;
sc_Renderer* renderer;
sc_Camera camera;

saci_Vec3 triangleVertices[3] = {
    {-1.0f, -1.0f, 0.0f}, // Vertex 1: bottom-left corner
    {1.0f, -1.0f, 0.0f},  // Vertex 2: bottom-right corner
    {0.0f, 1.0f, 0.0f}    // Vertex 3: top-center
};

static void init_saci() {
    saci_InitMath();
    assert(sc_GLFWInit());
    window = sc_CreateWindow(1600, 900, "SACI ROTATING-CUBE 3D", NULL, NULL);
    assert(window);
    sc_MakeWindowContext(window);
    assert(sc_GLADInit());

    renderer = sc_CreateRenderer(true);
    assert(renderer);

    camera = sc_GenerateDefaultCamera3D();
    camera.aspectRatio = 1600.0f / 900.0f;
    camera.position.z = -10.0f; // Change as you may
    camera.position.y = 5.0f;

    sc_RenderEnableZBuffer();
    sc_RenderSetProjectionMode(SACI_RENDER_PERSPECTIVE_PROJECTION);
}

int main() {
    init_saci();
    int tex = sc_TextureLoad("./texture/basic-img/cat-standing-up.png", false);
    saci_Color bgColor = saci_ColorFromU8(25, 70, 125, 255); // Colors are stored as float values from 0 to 1
    while (!sc_WindowShouldClose(window)) {
        sc_ClearWindow(bgColor);
        sc_RenderPushTriangleTexture(renderer, triangleVertices[0], triangleVertices[0], triangleVertices[0], tex);
        sc_RenderEnd(renderer, NULL);
        sc_SwapWindowBuffer(window);
        sc_PollEvents();
    }
}
