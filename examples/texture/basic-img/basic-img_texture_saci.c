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
    int tex = sc_TextureLoad("./texture/basic-img/cat-standing-up.png", false);
    while (!sc_WindowShouldClose(window)) {
    }
}
