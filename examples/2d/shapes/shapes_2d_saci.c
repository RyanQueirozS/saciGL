#include "saci-core/sc-event.h"
#include "saci-core/sc-gl.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"

#include <assert.h>

#include <unistd.h>

static sc_Window_t* window;
static sc_Renderer_t* renderer;

const int screen_width = 1600;
const int screen_height = 900;

sa_Vec3_t triangleVert[3] = {
    (sa_Vec3_t){.x = 1, .y = 0.4, 0},
    (sa_Vec3_t){.x = 0.8, .y = 0.7, 0},
    (sa_Vec3_t){.x = 0.3, .y = 0.1, 0},
};

sa_Color_t triangleColor[3] = {
    (sa_Color_t){1, 0, 0, 1},
    (sa_Color_t){0, 1, 0, 1},
    (sa_Color_t){0, 0, 1, 1},
};

sa_U32_t indices[3] = {
    0, // triangleVert[0]
    1, // triangleVert[1]
    2, // triangleVert[2]
};

sa_U64_t verticeAmount = 3;
sa_U64_t indiceAmount = 3;

int main() {
    {
        assert(sc_GLFW_Init());
        window = sc_Window_Create(screen_width, screen_height, "SACI SHAPES 2D", NULL, NULL);
        assert(window);
        sc_Window_Make_Context(window);
        assert(sc_GLAD_Init());
        sa_Math_Init();
    }

    // Generates defaulted shaders, vertex and array objects
    renderer = sc_Renderer_Create_Default();
    assert(renderer);

    sa_Color_t bgColor = sa_Color_From_U8(25, 70, 125, 255);
    struct sc_Vertice_c* vertices =
        sc_Vertice_Create_Vertices_Array(triangleVert, triangleColor, NULL, verticeAmount);

    sa_Mat4_t modelMatrix = sa_Mat4_Identity(); // Generate a defaulted mat4 as modelMatrix

    while (!sc_Window_Should_Close(window)) {
        sc_Window_Clear_Color(bgColor);

        sc_Renderer_Begin(renderer);
        sa_U32_t ibo = sc_GL_Create_Index_Buffer(indices, indiceAmount);
        sc_Renderer_Push_Vertices(renderer, vertices, verticeAmount, indiceAmount,
                                  modelMatrix, 0, ibo);
        sc_Renderer_End(renderer, NULL);
        sc_Window_Swap_Buffer(window);

        sc_Event_Poll();
    }
    sc_Renderer_Delete(renderer);
    sc_Window_Terminate();
}
