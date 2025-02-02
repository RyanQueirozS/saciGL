#include "saci-core/sc-event.h"
#include "saci-core/sc-gl.h"
#include "saci-utils/su-math.h"

#include <assert.h>
#include <stdio.h>

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

sc_window_t* window;
sc_renderer_t* renderer;
struct sc_camera_c camera;
const float cameraSpeed = 0.3f;

void init_saci(void) {
    sa_Math_Init();
    assert(sc_GLFW_Init());
    window = sc_Window_Create(1600, 900, "SACI ROTATING-CUBE 3D", NULL, NULL);
    assert(window);
    sc_Window_Make_Context(window);
    assert(sc_GLAD_Init());

    renderer = sc_Renderer_Create_Default();
    assert(renderer);

    camera = sc_Camera_Get_Default();
    camera.m_aspect_ratio = 1600.0f / 900.0f;

    sc_Renderer_Enable_Z_Buffer();
    sc_Renderer_Set_Projection_Mode(sa_RENDERER_PROJECTION_MODE_PERSPECTIVE);
}

void handle_keyboard(void) {
    sa_vec3_t forward = sa_Vec3_Normalize(sa_Vec3_Subtract(camera.m_target, camera.m_position));
    sa_vec3_t right = sa_Vec3_Normalize(sa_Vec3_Cross(forward, camera.m_up));
    if (sc_Event_Is_Key_Pressed(window, sc_KEY_W)) {
        camera.m_position = sa_Vec3_Add(camera.m_position, sa_Vec3_Scale(forward, cameraSpeed));
    }
    if (sc_Event_Is_Key_Pressed(window, sc_KEY_A)) {
        camera.m_position = sa_Vec3_Subtract(camera.m_position, sa_Vec3_Scale(right, cameraSpeed));
    }
    if (sc_Event_Is_Key_Pressed(window, sc_KEY_S)) {
        camera.m_position = sa_Vec3_Subtract(camera.m_position, sa_Vec3_Scale(forward, cameraSpeed));
    }
    if (sc_Event_Is_Key_Pressed(window, sc_KEY_D)) {
        camera.m_position = sa_Vec3_Add(camera.m_position, sa_Vec3_Scale(right, cameraSpeed));
    }
    if (sc_Event_Is_Key_Pressed(window, sc_KEY_SPACE)) {
        camera.m_position.m_y += cameraSpeed;
    }
    if (sc_Event_Is_Key_Pressed(window, sc_KEY_LEFT_SHIFT)) {
        camera.m_position.m_y -= cameraSpeed;
    }
}

void file_read(void* ctx, const char* filename, int isMtl, const char* objFilename, char** buf,
               size_t* len) {
    (void)ctx, (void)isMtl, (void)objFilename;
    // Open the file for reading
    FILE* file = fopen(filename, "rb");
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
    *buf = (char*)malloc(*len + 1); // +1 for null terminator
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

int main(void) {
    init_saci();
    sa_color_t bgColor =
        sa_Color_From_U8(25, 70, 125, 255); // Colors are stored as float values from 0 to 1

    struct sc_modelMesh_c* mesh;

    {
        const char* filePath = "./3d/stanford-bunny-model/bunny.obj";
        mesh = sc_Model_Mesh_Load(filePath);
    }

    assert(mesh);
    sa_mat4_t modelMatrix;
    sa_vec3_t modelPos = {0, 0, 0};
    sa_vec3_t modelRot = {0, 0, 0};
    sa_vec3_t modelScale = {1, 1, 1};
    camera.m_position.m_z = -3;
    camera.m_target = modelPos;

    modelMatrix = sa_Mat4_Model_Matrix(modelPos, modelRot, modelScale);

    while (!sc_Window_Should_Close(window)) {
        sc_Window_Clear_Color(bgColor);
        handle_keyboard();

        sc_Renderer_Begin(renderer);
        sc_Renderer_Push_Model_Mesh(renderer, mesh, modelMatrix, 0);
        sc_Renderer_End(renderer, &camera);
        sc_Window_Swap_Buffer(window);

        sc_Event_Poll();
    }
    sc_Model_Mesh_Delete(mesh);
    sc_Renderer_Delete(renderer);
    sc_Window_Free(window);
    sc_Window_Terminate();
}
