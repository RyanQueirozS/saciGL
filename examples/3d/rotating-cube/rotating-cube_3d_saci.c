#include <assert.h>
#include "saci-core/sc-camera.h"
#include "saci-core/sc-event.h"
#include "saci-core/sc-gl.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"
#include <saci-utils/su-general.h>
#include <stdlib.h>

struct sc_Camera_c camera;
sc_Renderer_t* renderer;
sc_Window_t* window;

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

void init_saci() {
    saci_InitMath();
    assert(sc_GLFW_Init());
    window = sc_Window_Create(1600, 900, "SACI ROTATING-CUBE 3D", NULL, NULL);
    assert(window);
    sc_Window_Make_Context(window);
    assert(sc_GLAD_Init());

    renderer = sc_Renderer_CreateDefault();
    assert(renderer);

    camera = sc_Camera_Get_Default();
    camera.aspectRatio = 1600.0f / 900.0f;
    camera.position.z = -10.0f; // Change as you may
    camera.position.y = 0.0f;

    sc_Renderer_EnableZBuffer();
    sc_Renderer_SetProjectionMode(sa_RENDER_PERSPECTIVE_PROJECTION);
}

int main() {
    init_saci();

    struct sc_Vertice_c* vertices = sc_Vertice_CreateVerticesArray(verticesPos, colors, NULL, 8);

    saci_Vec3 rotation = {0, 0, 0};
    saci_Mat4 modelMatrix =
        saci_Mat4_ModelMatrix((saci_Vec3){0, 0, 0}, rotation, (saci_Vec3){1, 1, 1});

    saci_Color bgColor =
        saci_ColorFromU8(25, 70, 125, 255); // Colors are stored as float values from 0 to 1
    while (!sc_Window_Should_Close(window)) {
        sc_Window_Clear_Color(bgColor);

        sc_Renderer_Begin(renderer);
        saci_u32 ibo = sc_GL_CreateIndexBuffer(cubeIndices, indiceAmount);
        sc_Renderer_PushVertices(renderer, vertices, verticeAmount, indiceAmount,
                                 modelMatrix, 0, ibo);
        sc_Renderer_End(renderer, &camera);
        sc_Window_Swap_Buffer(window);

        sc_Event_Poll();
        {
            rotation.x += 0.01;
            rotation.z += 0.01;
            rotation.y += 0.01;
            modelMatrix =
                saci_Mat4_ModelMatrix((saci_Vec3){0, 0, 0}, rotation, (saci_Vec3){1, 1, 1});
        }
    }
}
