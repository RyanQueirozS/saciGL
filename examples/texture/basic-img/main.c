#include "saci-core/sc-gl.h"
#include "saci-core/sc-texture.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"

#include <assert.h>
#include <stdio.h>

sc_window_t* window;
sc_renderer* renderer;

sa_vec3_t triangleVertices[4] = {
    {-1.0f, -1.0f, 0.0f}, // Vertex 1: bottom-left corner
    {1.0f, -1.0f, 0.0f},  // Vertex 2: bottom-right corner
    {-1.0f, 1.0f, 0.0f},  // Vertex 3: top-left corner
    {1.0f, 1.0f, 0.0f}    // Vertex 3: top-right corner
};

sa_color_t vertexColors[4] = {
    {1.0f, 0.0f, 0.0f, 1.0f}, // Red color for Vertex 1
    {0.0f, 1.0f, 0.0f, 1.0f}, // Green color for Vertex 2
    {0.0f, 0.0f, 1.0f, 1.0f}, // Blue color for Vertex 3
    {1.0f, 1.0f, 0.0f, 1.0f}  // Yellow color for Vertex 4
}; // NOTE: If you want the image to have it's "natural color", just set everything to
   // white aka {1.0f,1.0f,1.0f,1.0f}

sa_uv uvCoords[4] = {
    {0.0f, 0.0f}, // UV for Vertex 1 (bottom-left)
    {1.0f, 0.0f}, // UV for Vertex 2 (bottom-right)
    {0.0f, 1.0f}, // UV for Vertex 3 (top-left)
    {1.0f, 1.0f}  // UV for Vertex 4 (top-right)
};

sa_u32_t indices[6] = {
    0, 1, 2, // first triangle
    1, 2, 3, // second triangle
};

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

static void init_saci() {
    assert(sc_GLFW_Init());
    window = sc_Window_Create(1600, 900, "SACI ROTATING-CUBE 3D", NULL, NULL);
    assert(window);
    sc_Window_Make_Context(window);
    assert(sc_GLAD_Init());

    renderer = sc_Renderer_New_Default();
    sc_Renderer_Set_Uniform_Struct(renderer, sizeof(uniforms));
    uniforms.model = sa_Mat4_Model_Matrix((sa_vec3_t){0, 0, 0}, (sa_vec3_t){0, 0, 0}, (sa_vec3_t){1, 1, 1});
    uniforms.view = sa_Mat4_Look_At((sa_vec3_t){0.0f, 2.0f, -5.0f}, (sa_vec3_t){0.0f, 0.0f, 0.0f}, (sa_vec3_t){0.0f, 1.0f, 0.0f});
    uniforms.projection = sa_Mat4_Perspective(90, 16.0f / 9.0f, 1, 100);
    uniforms.flags |= sc_RENDERER_UNIFORM_FLAG_IS_3D;
    uniforms.lighting = (sa_vec4_t){0, 0, 0, 0};
    assert(renderer);
}

int main() {
    init_saci();
    int tex = sc_Texture_Load("./texture/basic-img/cat-standing-up.png", false);
    assert(tex);
    sa_color_t bgColor = sa_Color_From_U8(25, 70, 125, 255); // Colors are stored as float values from 0 to 1
    sa_mat4_t modelMatrix = sa_Mat4_Identity();
    while (!sc_Window_Should_Close(window)) {
        sc_Window_Clear_Color(bgColor);

        sc_Renderer_Begin(renderer);
        sc_Renderer_Bind_Uniform_Struct(renderer, (void*)&uniforms);
        sc_Renderer_Bind_Index_Buffer(renderer, indices, 6);
        sc_Renderer_Bind_Texture(renderer, tex);
        sc_Renderer_Push_Vertex(renderer, triangleVertices, uvCoords, vertexColors, 4);
        sc_Renderer_End(renderer);
        sc_Window_Swap_Buffer(window);

        sc_Event_Poll();
    }
    sc_Texture_Free(tex);
    sc_Renderer_Free(renderer);
    sc_Window_Terminate();
}
