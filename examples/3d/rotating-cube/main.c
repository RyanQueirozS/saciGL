#include "saci-core/sc-gl.h"
#include "saci-core/sc-windowing.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"

#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

sc_window_t* window;

// Define the 8 vertices of a cube centered at the origin with side length 2
sa_u64 verticesAmount = 8;
sa_vec3 verticesPos[] = {
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
sa_color cubeColors[] = {
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
sa_u64 indiceAmount = 36;
sa_u32 cubeIndices[] = {
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
    sa_mat4 model;
    sa_mat4 view;
    sa_mat4 projection;
    sa_s32 flags;
    sa_vec4 lighting;
} uniforms;

static sa_vec3 pos[3] = {
    (sa_vec3){0, 0, 0},
    (sa_vec3){8, 1, 5},
    (sa_vec3){3, 0, -15},
};

static sa_vec3 rotation[3] = {};

void init_saci() {
    assert(sc_GLFW_Init());
    window = sc_Window_Create(1600, 900, "SACI ROTATING-CUBE 3D", NULL, NULL);
    assert(window);
    sc_Window_Make_Context(window);
    assert(sc_GLAD_Init());

    rendr = sc_Renderer_New_Default();

    uniforms.view = sa_Mat4_Look_At((sa_vec3){0.0f, 2.0f, -20.0f}, (sa_vec3){0.0f, 0.0f, 0.0f}, (sa_vec3){0.0f, 1.0f, 0.0f});
    uniforms.projection = sa_Mat4_Perspective(90, 16.0f / 9.0f, 1, 100);
    uniforms.flags |= sc_RENDERER_UNIFORM_FLAG_IS_3D;
    uniforms.lighting = (sa_vec4){0, 0, 0, 0};
}

int main() {
    init_saci();

    sa_color bgColor =
        sa_Color_From_U8(25, 70, 125, 255); // Colors are stored as float values from 0 to 1

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_FALSE);
    sa_u32 proj_loc = sc_Renderer_Get_Uniform_Id(rendr, "u_projection_matrix");
    sa_u32 view_loc = sc_Renderer_Get_Uniform_Id(rendr, "u_view_matrix");
    sa_u32 model_loc = sc_Renderer_Get_Uniform_Id(rendr, "u_model_matrix");

    sa_dArray* indices = sa_DArray_Create(36, sizeof(sa_u32), sa_TRUE);
    for (sa_u32 i = 0; i < indiceAmount; ++i) {
        sa_DArray_Push(indices, &cubeIndices[i]);
    }
    sa_dArray* positions = sa_DArray_Create(8, sizeof(sa_vec3), sa_TRUE);
    for (sa_u32 i = 0; i < sa_DArray_Capacity(positions); ++i) {
        sa_DArray_Push(positions, &verticesPos[i]);
    }

    sa_dArray* colors = sa_DArray_Create(36, sizeof(sa_color), sa_TRUE);
    for (sa_u32 i = 0; i < 36; ++i) {
        sa_DArray_Push(colors, &cubeColors[i]);
    }

    sa_dArray* translations = sa_DArray_Create(3, sizeof(sa_mat4), sa_TRUE);
    sa_mat4 mat1 = sa_Mat4_Model_Matrix_RTS(pos[0], rotation[0], (sa_vec3){1, 1, 1});
    sa_mat4 mat2 = sa_Mat4_Model_Matrix_RTS(pos[1], rotation[1], (sa_vec3){1, 1, 1});
    sa_mat4 mat3 = sa_Mat4_Model_Matrix_RTS(pos[2], rotation[2], (sa_vec3){1, 1, 1});
    sa_DArray_Push(translations, &mat1);
    sa_DArray_Push(translations, &mat2);
    sa_DArray_Push(translations, &mat3);

    while (!sc_Window_Should_Close(window)) {
        sc_Event_Poll();
        sc_Window_Clear_Color(bgColor);

        sc_Renderer_Begin(rendr);

        sc_Renderer_Set_Uniform(rendr, proj_loc, &uniforms.projection, SA_TYPE_MAT4);
        sc_Renderer_Set_Uniform(rendr, view_loc, &uniforms.view, SA_TYPE_MAT4);
        sc_Renderer_Set_Uniform(rendr, model_loc, &uniforms.model, SA_TYPE_MAT4);
        sc_Renderer_Bind_Index_Buffer(rendr, indices);
        sc_Renderer_Push_Mesh_Instanced(rendr, positions, NULL, colors, translations);
        sc_Renderer_End(rendr);
        sc_Window_Swap_Buffer(window);
        {
            rotation[0].x += 0.01;
            rotation[0].z += 0.01;
            rotation[0].y += 0.01;

            rotation[1].x += 0.02;
            rotation[1].z += 0.01;
            rotation[1].y += 0.08;

            rotation[2].x += 0.01;
            rotation[2].z += 0.02;
            rotation[2].y += 0.03;
        }
    }
    sc_Renderer_Free(rendr);
    sc_Window_Free(window);
    sc_Window_Terminate();
}
