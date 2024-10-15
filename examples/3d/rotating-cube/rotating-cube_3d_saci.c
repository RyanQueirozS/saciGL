#include <saci-core.h>
#include <saci-core/sc-windowing.h>

#include <assert.h>
#include "saci-core/sc-camera.h"
#include "saci-core/sc-event.h"
#include "saci-core/sc-rendering.h"
#include "saci-utils/su-math.h"
#include <saci-utils/su-general.h>

#define SACI_CORE_IMPLEMENTATION
// #define SACI_LIB_IMPLEMENTATION

#ifdef SACI_CORE_IMPLEMENTATION
static sc_Camera camera;
static sc_Renderer* renderer;
static sc_Window* window;

// Define the 8 vertices of a cube centered at the origin with side length 2
static saci_Vec3 vertices[] = {
    {-1.0f, -1.0f, -1.0f}, // v0: Bottom-left-back
    {1.0f, -1.0f, -1.0f},  // v1: Bottom-right-back
    {1.0f, 1.0f, -1.0f},   // v2: Top-right-back
    {-1.0f, 1.0f, -1.0f},  // v3: Top-left-back
    {-1.0f, -1.0f, 1.0f},  // v4: Bottom-left-front
    {1.0f, -1.0f, 1.0f},   // v5: Bottom-right-front
    {1.0f, 1.0f, 1.0f},    // v6: Top-right-front
    {-1.0f, 1.0f, 1.0f}    // v7: Top-left-front
};

// Define colors for each vertex
static saci_Color colors[] = {
    {1.0f, 0.0f, 0.0f, 1.0f}, // color0: Red
    {0.0f, 1.0f, 0.0f, 1.0f}, // color1: Green
    {0.0f, 0.0f, 1.0f, 1.0f}, // color2: Blue
    {1.0f, 1.0f, 0.0f, 1.0f}, // color3: Yellow
    {1.0f, 0.0f, 1.0f, 1.0f}, // color4: Magenta
    {0.0f, 1.0f, 1.0f, 1.0f}, // color5: Cyan
    {1.0f, 0.5f, 0.0f, 1.0f}, // color6: Orange
    {0.5f, 0.0f, 1.0f, 1.0f}  // color7: Purple
};

// Define the indices for the triangles of each face of the cube
static int triangles[][6] = {
    // Front face (v4, v5, v6, v7)
    {4, 5, 6, 4, 6, 7},
    // Back face (v0, v1, v2, v3)
    {0, 1, 2, 0, 2, 3},
    // Left face (v0, v3, v7, v4)
    {0, 3, 7, 0, 7, 4},
    // Right face (v1, v2, v6, v5)
    {1, 2, 6, 1, 6, 5},
    // Top face (v3, v2, v6, v7)
    {3, 2, 6, 3, 6, 7},
    // Bottom face (v0, v1, v5, v4)
    {0, 1, 5, 0, 5, 4}};

static void draw_cube() {
    // Loop through the triangles and render them
    for (int i = 0; i < 6; ++i) {
        // First triangle of the face
        sc_RenderPushTriangle3D(renderer,
                                vertices[triangles[i][0]],
                                vertices[triangles[i][1]],
                                vertices[triangles[i][2]],
                                colors[triangles[i][0]],
                                colors[triangles[i][1]],
                                colors[triangles[i][2]]);

        // Second triangle of the face
        sc_RenderPushTriangle3D(renderer,
                                vertices[triangles[i][3]],
                                vertices[triangles[i][4]],
                                vertices[triangles[i][5]],
                                colors[triangles[i][3]],
                                colors[triangles[i][4]],
                                colors[triangles[i][5]]);
    }
}

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

    saci_Color bgColor = saci_ColorFromU8(25, 70, 125, 255); // Colors are stored as float values from 0 to 1
    while (!sc_WindowShouldClose(window)) {
        sc_ClearWindow(bgColor);

        sc_RenderBegin(renderer);
        draw_cube();
        sc_RenderEnd(renderer, &camera);
        sc_SwapWindowBuffer(window);

        sc_PollEvents();
    }
}

#endif

#ifdef SACI_LIB_IMPLEMENTATION
#endif
