#include <saci-core.h>
#include <saci-core/sc-windowing.h>

#include <assert.h>
#include "saci-core/sc-camera.h"
#include "saci-core/sc-event.h"
#include "saci-core/sc-rendering.h"
#include "saci-utils/su-math.h"
#include <saci-utils/su-general.h>
#include <stdlib.h>

sc_Camera camera;
sc_Renderer* renderer;
sc_Window* window;

// Define the 8 vertices of a cube centered at the origin with side length 2
saci_u64 verticeAmount = 8;
saci_Vec3 verticesPos[] = {
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
saci_Color colors[] = {
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
saci_u64 indiceAmount = 36;
saci_u32 cubeIndices[] = {
    0, 1, 2, 0, 2, 3, // Back face
    4, 5, 6, 4, 6, 7, // Front face
    0, 1, 5, 0, 5, 4, // Bottom face
    2, 3, 7, 2, 7, 6, // Top face
    0, 3, 7, 0, 7, 4, // Left face
    1, 2, 6, 1, 6, 5  // Right face
};

sc_Vertice* createCubeData(saci_Vec3* verticesPos, saci_u64 verticeAmount) {
    sc_Vertice* vertices = (sc_Vertice*)malloc(verticeAmount * sizeof(sc_Vertice));

    for (saci_u64 i = 0; i < verticeAmount; ++i) {
        vertices[i].pos = verticesPos[i];
        vertices[i].color = colors[i];
        vertices[i].texCoord = (saci_Vec2){0.0f, 0.0f}; // Texture coordinates are not used
    }
    return vertices;
}

void init_saci() {
    saci_InitMath();
    assert(sc_GLFW_Init());
    window = sc_Window_Create(1600, 900, "SACI ROTATING-CUBE 3D", NULL, NULL);
    assert(window);
    sc_Window_MakeContext(window);
    assert(sc_GLAD_Init());

    renderer = sc_Renderer_Create(true);
    assert(renderer);

    camera = sc_Camera_GetDefault3DCamera();
    camera.aspectRatio = 1600.0f / 900.0f;
    camera.position.z = -10.0f; // Change as you may
    camera.position.y = 5.0f;

    sc_Renderer_EnableZBuffer();
    sc_Renderer_SetProjectionMode(SACI_RENDER_PERSPECTIVE_PROJECTION);
}

int main() {
    init_saci();

    sc_Vertice* vertices = createCubeData(verticesPos, verticeAmount);

    saci_Color bgColor =
        saci_ColorFromU8(25, 70, 125, 255); // Colors are stored as float values from 0 to 1
    while (!sc_Window_ShouldClose(window)) {
        sc_Window_ClearColor(bgColor);

        sc_Renderer_Begin(renderer);
        sc_Renderer_PushVertices(renderer, vertices, verticeAmount, cubeIndices, indiceAmount, 0);
        sc_Renderer_End(renderer, &camera);
        sc_Window_SwapBuffer(window);

        sc_Event_Poll();
    }
}
