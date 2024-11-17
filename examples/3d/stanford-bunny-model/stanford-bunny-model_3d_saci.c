#include "saci-core/sc-event.h"
#include "saci-core/sc-gl.h"
#include "saci-utils/su-math.h"

#include <assert.h>
#include <stdio.h>

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

sc_Window *window;
sc_Renderer *renderer;
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
    sc_Renderer_SetNoFillMode();
}

void file_read(void *ctx, const char *filename, int isMtl, const char *objFilename, char **buf,
               size_t *len) {
    (void)ctx, (void)isMtl, (void)objFilename;
    // Open the file for reading
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        *buf = NULL;
        *len = 0;
        return;
    }

    // Seek to the end to get the file size
    fseek(file, 0, SEEK_END);
    *len = ftell(file);
    if (*len == 0) {
        *buf = NULL; // No need to allocate anything if the file is empty.
    }
    rewind(file);

    // Allocate memory for the buffer
    *buf = (char *)malloc(*len + 1); // +1 for null terminator
    if (!*buf) {
        fprintf(stderr, "Failed to allocate memory\n");
        fclose(file);
        *len = 0;
        return;
    }

    // Read file contents into buffer
    fread(*buf, 1, *len, file);
    (*buf)[*len] = '\0'; // Null terminate the buffer

    fclose(file);
}

int main() {
    init_saci();
    saci_Color bgColor =
        saci_ColorFromU8(25, 70, 125, 255); // Colors are stored as float values from 0 to 1

    sc_ModelMesh *mesh;

    {
        const char *filePath = "./3d/stanford-bunny-model/bunny.obj";
        sc_OBJ_ModelFileReadingFunction func = file_read;

        mesh = sc_ModelMesh_Load(filePath, func);
    }

    assert(mesh);
    saci_Mat4 modelMatrix;
    saci_Vec3 modelPos = {0, 0, 0};
    saci_Vec3 modelRot = {0, 0, 0};
    saci_Vec3 modelScale = {1, 1, 1};
    modelMatrix = saci_Mat4_ModelMatrix(modelPos, modelRot, modelScale);
    while (!sc_Window_ShouldClose(window)) {
        sc_Window_ClearColor(bgColor);

        sc_Renderer_Begin(renderer);
        sc_Renderer_PushModelMesh(renderer, mesh, modelMatrix, 0);
        sc_Renderer_End(renderer, &camera);
        sc_Window_SwapBuffer(window);

        sc_Event_Poll();
    }
    sc_ModelMesh_Delete(mesh);
}
