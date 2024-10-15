
#include "saci-core/sc-event.h"
#include "saci-core/sc-rendering.h"
#include "saci-core/sc-windowing.h"
#include "saci-utils/su-math.h"

#include <assert.h>

#include <unistd.h>

static sc_Window* window;
static sc_Renderer* renderer;

static const int screen_width = 1600;
static const int screen_height = 900;

static saci_Vec2 triangleVert[3] = {
    (saci_Vec2){.x = 1, .y = 0.4},
    (saci_Vec2){.x = 0.8, .y = 0.7},
    (saci_Vec2){.x = 0.3, .y = 0.1},
};

static saci_Color triangleColor[3] = {
    (saci_Color){1, 0, 0, 1},
    (saci_Color){0, 1, 0, 1},
    (saci_Color){0, 0, 1, 1},
};

int main() {
    {
        assert(sc_GLFWInit());
        window = sc_CreateWindow(1600, 900, "SACI SHAPES 2D", NULL, NULL);
        assert(window);
        sc_MakeWindowContext(window);
        assert(sc_GLADInit());
        saci_InitMath();
    }

    // Generates defaulted shaders, vertex and array objects
    renderer = sc_CreateRenderer(true);
    assert(renderer);

    saci_Color bgColor = saci_ColorFromU8(25, 70, 125, 255);
    while (!sc_WindowShouldClose(window)) {
        sc_ClearWindow(bgColor);

        sc_RenderBegin(renderer);
        sc_RenderPushTriangle2D(renderer, triangleVert[0], triangleVert[1], triangleVert[2], 0.0f, triangleColor[0], triangleColor[1], triangleColor[2]);
        sc_RenderEnd(renderer, NULL);
        sc_SwapWindowBuffer(window);

        sc_PollEvents();
    }
    sc_DeleteRenderer(renderer);
    sc_Terminate();
}
