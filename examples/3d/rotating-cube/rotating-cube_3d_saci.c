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
sa_U64_t verticeAmount = 8;
sa_Vec3_t verticesPos[] = {
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
sa_Color_t colors[] = {
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
sa_U64_t indiceAmount = 36;
sa_U32_t cubeIndices[] = {
    0, 1, 2, 0, 2, 3, // Back face
    4, 5, 6, 4, 6, 7, // Front face
    0, 1, 5, 0, 5, 4, // Bottom face
    2, 3, 7, 2, 7, 6, // Top face
    0, 3, 7, 0, 7, 4, // Left face
    1, 2, 6, 1, 6, 5  // Right face
};

void init_saci() {
    sa_Math_Init();
    assert(sc_GLFW_Init());
    window = sc_Window_Create(1600, 900, "SACI ROTATING-CUBE 3D", NULL, NULL);
    assert(window);
    sc_Window_Make_Context(window);
    assert(sc_GLAD_Init());

    renderer = sc_Renderer_Create_Default();
    assert(renderer);

    camera = sc_Camera_Get_Default();
    camera.aspectRatio = 1600.0f / 900.0f;
    camera.position.z = -10.0f; // Change as you may
    camera.position.y = 0.0f;

    sc_Renderer_Enable_Z_Buffer();
    sc_Renderer_Set_Projection_Mode(sa_RENDERER_PROJECTION_MODE_PERSPECTIVE);
}

int main() {
    init_saci();

    struct sc_Vertice_c* vertices = sc_Vertice_Create_Vertices_Array(verticesPos, colors, NULL, 8);

    sa_Vec3_t rotation = {0, 0, 0};
    sa_Mat4_t modelMatrix =
        sa_Mat4_Model_Matrix((sa_Vec3_t){0, 0, 0}, rotation, (sa_Vec3_t){1, 1, 1});

    sa_Color_t bgColor =
        sa_Color_From_U8(25, 70, 125, 255); // Colors are stored as float values from 0 to 1
    while (!sc_Window_Should_Close(window)) {
        sc_Window_Clear_Color(bgColor);

        sc_Renderer_Begin(renderer);
        sa_U32_t ibo = sc_GL_Create_Index_Buffer(cubeIndices, indiceAmount);
        sc_Renderer_Push_Vertices(renderer, vertices, verticeAmount, indiceAmount,
                                  modelMatrix, 0, ibo);
        sc_Renderer_End(renderer, &camera);
        sc_Window_Swap_Buffer(window);

        sc_Event_Poll();
        {
            rotation.x += 0.01;
            rotation.z += 0.01;
            rotation.y += 0.01;
            modelMatrix = sa_Mat4_Model_Matrix((sa_Vec3_t){0, 0, 0}, rotation, (sa_Vec3_t){1, 1, 1});
        }
    }
}
