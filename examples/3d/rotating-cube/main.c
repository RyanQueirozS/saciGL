#include "saci-core/sc-gl.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"

#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

sc_window_t* window;

// Define the 8 vertices of a cube centered at the origin with side length 2
sa_u64_t verticeAmount = 8;
sa_vec3_t verticesPos[] = {
    {-1.0f, -1.0f, -1.0f}, // v0: Bottom-left-back
    {1.0f, -1.0f, -1.0f},  // v1: Bottom-right-back
    {1.0f, 1.0f, -1.0f},   // v2: Top-right-back
    {-1.0f, 1.0f, -1.0f},  // v3: Top-left-back
    {-1.0f, -1.0f, 1.0f},  // v4: Bottom-left-front
    {1.0f, -1.0f, 1.0f},   // v5: Bottom-right-front
    {1.0f, 1.0f, 1.0f},    // v6: Top-right-front
    {-1.0f, 1.0f, 1.0f}    // v7: Top-left-front
};

sa_uv verticesUV[8] = {0};

// Define colors for each vertex
sa_color_t colors[] = {
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
sa_u64_t indiceAmount = 36;
sa_u32_t cubeIndices[] = {
    0, 1, 2, 0, 2, 3, // Back face
    4, 5, 6, 4, 6, 7, // Front face
    0, 1, 5, 0, 5, 4, // Bottom face
    2, 3, 7, 2, 7, 6, // Top face
    0, 3, 7, 0, 7, 4, // Left face
    1, 2, 6, 1, 6, 5  // Right face
};

sc_renderer* rendr;

// saciCore accepts a structure to handle all uniform data, this doesn't need
// to be aligned as saciCore aligns it to std140 itself. All data under the default
// shader MUST follow this order since the first 64 bytes are the model, then
// view, then projection, 4 bytes for flags and 16 bytes for lighting :
struct uniforms {
    sa_mat4_t model;
    sa_mat4_t view;
    sa_mat4_t projection;
    sa_s32_t flags;
    sa_vec4_t lighting;
} uniforms;

int offset_model, offset_view, offset_projection, offset_flags, offset_lighting;

static sa_vec3_t rotation = {0};

void init_saci() {
    assert(sc_GLFW_Init());
    window = sc_Window_Create(1600, 900, "SACI ROTATING-CUBE 3D", NULL, NULL);
    assert(window);
    sc_Window_Make_Context(window);
    assert(sc_GLAD_Init());

    rendr = sc_Renderer_New_Default();

    sc_Renderer_Set_Uniform_Struct(rendr, sizeof(struct uniforms));
    uniforms.model = sa_Mat4_Model_Matrix((sa_vec3_t){0, 0, 0}, rotation, (sa_vec3_t){1, 1, 1});
    uniforms.view = sa_Mat4_Look_At((sa_vec3_t){0, -5, 0}, (sa_vec3_t){0, 0, 0}, (sa_vec3_t){0, 1, 0});
    uniforms.projection = sa_Mat4_Perspective(90, 16.0f / 9.0f, 1, 100);
    uniforms.flags |= sc_RENDERER_UNIFORM_FLAG_IS_3D;
    uniforms.lighting = (sa_vec4_t){0, 0, 0, 0};
    // sc_Renderer_Enable_Z_Buffer();
    // sc_Renderer_Set_Projection_Mode(sa_RENDERER_PROJECTION_MODE_PERSPECTIVE);
}

int main() {
    init_saci();

    sa_color_t bgColor =
        sa_Color_From_U8(25, 70, 125, 255); // Colors are stored as float values from 0 to 1

    while (!sc_Window_Should_Close(window)) {
        sc_Window_Clear_Color(bgColor);
        sc_Renderer_Begin(rendr);
        sc_Window_Clear_Color(bgColor);
        sc_Renderer_Bind_Uniform_Struct(rendr, (void*)&uniforms);
        sc_Renderer_Bind_Index_Buffer(rendr, cubeIndices, indiceAmount);
        sc_Renderer_Push_Vertex(rendr, verticesPos, verticesUV, colors, verticeAmount);
        sc_Renderer_End(rendr);
        sc_Window_Swap_Buffer(window);

        sc_Event_Poll();
        {
            rotation.m_x += 0.03;
            rotation.m_z += 0.03;
            rotation.m_y += 0.03;
            uniforms.model = sa_Mat4_Model_Matrix((sa_vec3_t){0, 0, 0}, rotation, (sa_vec3_t){1, 1, 1});
        }
    }
}
