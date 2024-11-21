#include "saci-core/sc-event.h"
#include "saci-core/sc-gl.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"

#include <assert.h>

#include <unistd.h>

static sc_Window* window;
static sc_Renderer* renderer;

const int screen_width = 1600;
const int screen_height = 900;

saci_Vec3 triangleVert[3] = {
    (saci_Vec3){.x = 1, .y = 0.4, 0},
    (saci_Vec3){.x = 0.8, .y = 0.7, 0},
    (saci_Vec3){.x = 0.3, .y = 0.1, 0},
};

saci_Color triangleColor[3] = {
    (saci_Color){1, 0, 0, 1},
    (saci_Color){0, 1, 0, 1},
    (saci_Color){0, 0, 1, 1},
};

saci_u32 indices[3] = {
    0, // triangleVert[0]
    1, // triangleVert[1]
    2, // triangleVert[2]
};

saci_u64 verticeAmount = 3;
saci_u64 indiceAmount = 3;

int main() {
    {
        assert(sc_GLFW_Init());
        window = sc_Window_Create(screen_width, screen_height, "SACI SHAPES 2D", NULL, NULL);
        assert(window);
        sc_Window_MakeContext(window);
        assert(sc_GLAD_Init());
        saci_InitMath();
    }

    // Generates defaulted shaders, vertex and array objects
    renderer = sc_Renderer_Create(true);
    assert(renderer);

    saci_Color bgColor = saci_ColorFromU8(25, 70, 125, 255);
    sc_Vertice* vertices =
        sc_Vertice_CreateVerticesArray(triangleVert, triangleColor, NULL, verticeAmount);

    saci_Mat4 modelMatrix = saci_IdentityMat4(); // Generate a defaulted mat4 as modelMatrix

    while (!sc_Window_ShouldClose(window)) {
        sc_Window_ClearColor(bgColor);

        sc_Renderer_Begin(renderer);
        saci_u32 ibo = sc_GL_CreateIndexBuffer(indices, indiceAmount);
        sc_Renderer_PushVertices(renderer, vertices, verticeAmount, indices, indiceAmount,
                                 modelMatrix, 0, ibo);
        sc_Renderer_End(renderer, NULL);
        sc_Window_SwapBuffer(window);

        sc_Event_Poll();
    }
    sc_Renderer_Delete(renderer);
    sc_Window_Terminate();
}
