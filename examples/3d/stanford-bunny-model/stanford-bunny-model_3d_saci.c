#include "saci-core/sc-event.h"
#include "saci-core/sc-gl.h"
#include "saci-utils/su-math.h"

#include <assert.h>
#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>

sc_Window* window;
sc_Renderer* renderer;
sc_Camera camera;

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
    camera.position.z = 5.0f; // Change as you may
    camera.position.y = 0.0f;

    sc_Renderer_EnableZBuffer();
    sc_Renderer_SetProjectionMode(SACI_RENDER_PERSPECTIVE_PROJECTION);
    // sc_Renderer_SetNoFillMode();
}

void read_file(const char* path, char** buffer, saci_u64* length) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", path);
        *buffer = NULL;
        *length = 0;
        return;
    }

    // Seek to the end to determine the file size
    fseek(file, 0, SEEK_END);
    *length = (saci_u64)ftell(file); // Get the size of the file
    fseek(file, 0, SEEK_SET);        // Go back to the beginning

    // Allocate a buffer to hold the contents of the file
    *buffer = (char*)malloc(*length);
    if (*buffer == NULL) {
        fprintf(stderr, "Error: Could not allocate memory\n");
        *length = 0;
        fclose(file);
        return;
    }

    // Read the file into the buffer
    fread(*buffer, 1, *length, file);

    // Close the file
    fclose(file);
}

int main() {
    init_saci();
    saci_Color bgColor =
        saci_ColorFromU8(25, 70, 125, 255); // Colors are stored as float values from 0 to 1

    sc_ModelMesh* mesh;

    {
        const char* filePath = "./3d/stanford-bunny-model/bunny.obj";
        sc_FileReadingFunction func = read_file;

        mesh = sc_ModelMesh_Load(filePath, func);
        printf("oi\n");
    }

    assert(mesh);
    saci_Mat4 modelMatrix;
    saci_Vec3 modelPos = {1, 0, 1};
    saci_Vec3 modelRot = {0, 0, 0};
    saci_Vec3 modelScale = {1, 1, 1};
    modelMatrix = saci_Mat4_ModelMatrix(modelPos, modelRot, modelScale);
    while (!sc_Window_ShouldClose(window)) {
        sc_Window_ClearColor(bgColor);

        sc_Renderer_Begin(renderer);
        sc_Renderer_PushModelMesh(renderer, mesh, modelMatrix, 0);
        sc_Renderer_End(renderer, &camera);
        camera.position.z -= 0.003;
        sc_Window_SwapBuffer(window);

        sc_Event_Poll();
    }
}
