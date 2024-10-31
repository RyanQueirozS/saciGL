
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

static saci_Vec3 triangleVert[3] = {
    (saci_Vec3){.x = 1, .y = 0.4, 0},
    (saci_Vec3){.x = 0.8, .y = 0.7, 0},
    (saci_Vec3){.x = 0.3, .y = 0.1, 0},
};

static saci_Color triangleColor[3] = {
    (saci_Color){1, 0, 0, 1},
    (saci_Color){0, 1, 0, 1},
    (saci_Color){0, 0, 1, 1},
};

int main() {
    {
        assert(sc_GLFW_Init());
        window = sc_Window_Create(1600, 900, "SACI SHAPES 2D", NULL, NULL);
        assert(window);
        sc_Window_MakeContext(window);
        assert(sc_GLAD_Init());
        saci_InitMath();
    }

    // Generates defaulted shaders, vertex and array objects
    renderer = sc_Renderer_Create(true);
    assert(renderer);

    saci_Color bgColor = saci_ColorFromU8(25, 70, 125, 255);
    sc_Vertice vertices[3] = {
        {triangleVert[0], triangleColor[0], {0, 0}}, // We do not use textures,
        {triangleVert[1], triangleColor[1], {0, 0}}, // so texcoord will be 0,0
        {triangleVert[2], triangleColor[2], {0, 0}},
    };
    saci_u32 indices[3] = {0, 1, 2}; // indices for the triangle
    while (!sc_Window_ShouldClose(window)) {
        sc_Window_ClearColor(bgColor);

        sc_Renderer_Begin(renderer);
        sc_Renderer_PushVertices(renderer, vertices, 3, indices, 3, 0);
        sc_Renderer_End(renderer, NULL);
        sc_Window_SwapBuffer(window);

        sc_Event_Poll();
    }
    sc_Renderer_Delete(renderer);
    sc_Window_Terminate();
}
