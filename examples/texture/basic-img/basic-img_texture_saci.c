#include "saci-core/sc-camera.h"
#include "saci-core/sc-event.h"
#include "saci-core/sc-rendering.h"
#include "saci-core/sc-texture.h"
#include "saci-core/sc-windowing.h"
#include "saci-utils/su-math.h"
#include "saci-utils/su-types.h"

#include <assert.h>
#include <stdio.h>

sc_Window* window;
sc_Renderer* renderer;
sc_Camera camera;

saci_Vec3 triangleVertices[4] = {
    {-1.0f, -1.0f, 0.0f}, // Vertex 1: bottom-left corner
    {1.0f, -1.0f, 0.0f},  // Vertex 2: bottom-right corner
    {-1.0f, 1.0f, 0.0f},  // Vertex 3: top-left corner
    {1.0f, 1.0f, 0.0f}    // Vertex 3: top-right corner
};

saci_Color vertexColors[4] = {
    {1.0f, 0.0f, 0.0f, 1.0f}, // Red color for Vertex 1
    {0.0f, 1.0f, 0.0f, 1.0f}, // Green color for Vertex 2
    {0.0f, 0.0f, 1.0f, 1.0f}, // Blue color for Vertex 3
    {1.0f, 1.0f, 0.0f, 1.0f}  // Yellow color for Vertex 4
}; // NOTE: If you want the image to have it's "natural color", just set everything to
   // white aka {1.0f,1.0f,1.0f,1.0f}

saci_Vec2 uvCoords[4] = {
    {0.0f, 0.0f}, // UV for Vertex 1 (bottom-left)
    {1.0f, 0.0f}, // UV for Vertex 2 (bottom-right)
    {0.0f, 1.0f}, // UV for Vertex 3 (top-left)
    {1.0f, 1.0f}  // UV for Vertex 4 (top-right)
};

static void init_saci() {
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
    { // Change as you may
        camera.position.x = -1.0f;
        camera.position.y = 3.0f;
        camera.position.z = 5.0f;
    }

    sc_Renderer_EnableZBuffer();
    sc_Renderer_SetProjectionMode(SACI_RENDER_PERSPECTIVE_PROJECTION);
}

int main() {
    init_saci();
    int tex = sc_Texture_Load("./texture/basic-img/cat-standing-up.png", false);
    assert(tex);
    saci_Color bgColor = saci_ColorFromU8(
        25, 70, 125, 255); // Colors are stored as float values from 0 to 1
    while (!sc_Window_ShouldClose(window)) {
        sc_Window_ClearColor(bgColor);

        sc_Renderer_Begin(renderer);
        sc_Renderer_PushTriangleTexture(renderer, triangleVertices[0],
                                        triangleVertices[1], triangleVertices[2],
                                        vertexColors[0], vertexColors[1], vertexColors[2],
                                        uvCoords[0], uvCoords[1], uvCoords[2], tex);
        sc_Renderer_PushTriangleTexture(renderer, triangleVertices[2],
                                        triangleVertices[1],
                                        triangleVertices[3], // Vertices
                                        vertexColors[2], vertexColors[1],
                                        vertexColors[3],                       // Colors
                                        uvCoords[2], uvCoords[1], uvCoords[3], // UVs
                                        tex);
        sc_Renderer_End(renderer, &camera);
        sc_Window_SwapBuffer(window);

        sc_Event_Poll();
    }
    sc_Texture_Free(tex);
    sc_Renderer_Delete(renderer);
    sc_Window_Terminate();
}
