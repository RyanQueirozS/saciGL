#include "saci-core/sc-camera.h"
#include "saci-core/sc-rendering.h"
#include "saci-core/sc-texture.h"
#include "saci-core/sc-windowing.h"
#include "saci-utils/su-math.h"

#include <assert.h>

sc_Window* window;
sc_Renderer* renderer;
sc_Camera camera;

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
    sc_TextureData tex = sc_LoadImage("./texture/basic-img/cat-standing-up.png");
    sc_BindTexture2D(tex, 0);
    while (!sc_WindowShouldClose(window)) {
    }
}
