#define __SC_RENDERER_DEFAULT_BATCH_VERTEX_CAPACITY 730000
#include "saci-core/sc-gl.h"
#include "saci-utils/su-math.h"

#include <assert.h>
#include <stdio.h>

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

sc_window_t* window;
sc_renderer* renderer;
const float cameraSpeed = 0.3f;

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

void init_saci(void) {
    assert(sc_GLFW_Init());
    window = sc_Window_Create(1600, 900, "SACI ROTATING-CUBE 3D", NULL, NULL);
    assert(window);
    sc_Window_Make_Context(window);
    assert(sc_GLAD_Init());

    renderer = sc_Renderer_New_Default();

    sc_Renderer_Set_Uniform_Struct(renderer, sizeof(uniforms));
    uniforms.model = sa_Mat4_Identity();
    uniforms.view = sa_Mat4_Look_At((sa_vec3){0.0f, 2.0f, -5.0f}, (sa_vec3){0.0f, 0.0f, 0.0f}, (sa_vec3){0.0f, 1.0f, 0.0f});
    uniforms.projection = sa_Mat4_Perspective(90, 16.0f / 9.0f, 1, 100);
    uniforms.flags |= sc_RENDERER_UNIFORM_FLAG_IS_3D;
    uniforms.lighting = (sa_vec4){0, 0, 0, 0};

    sc_Renderer_Set_Bound_Index_Buffer_Capacity(renderer, 440000);
}

// void handle_keyboard(void) {
//     sa_vec3_t forward = sa_Vec3_Normalize(sa_Vec3_Subtract(camera.m_target, camera.m_position));
//     sa_vec3_t right = sa_Vec3_Normalize(sa_Vec3_Cross(forward, camera.m_up));
//     if (sc_Event_Is_Key_Pressed(window, sc_KEY_W)) {
//         camera.m_position = sa_Vec3_Add(camera.m_position, sa_Vec3_Scale(forward, cameraSpeed));
//     }
//     if (sc_Event_Is_Key_Pressed(window, sc_KEY_A)) {
//         camera.m_position = sa_Vec3_Subtract(camera.m_position, sa_Vec3_Scale(right, cameraSpeed));
//     }
//     if (sc_Event_Is_Key_Pressed(window, sc_KEY_S)) {
//         camera.m_position = sa_Vec3_Subtract(camera.m_position, sa_Vec3_Scale(forward, cameraSpeed));
//     }
//     if (sc_Event_Is_Key_Pressed(window, sc_KEY_D)) {
//         camera.m_position = sa_Vec3_Add(camera.m_position, sa_Vec3_Scale(right, cameraSpeed));
//     }
//     if (sc_Event_Is_Key_Pressed(window, sc_KEY_SPACE)) {
//         camera.m_position.m_y += cameraSpeed;
//     }
//     if (sc_Event_Is_Key_Pressed(window, sc_KEY_LEFT_SHIFT)) {
//         camera.m_position.m_y -= cameraSpeed;
//     }
// }

int main(void) {
    init_saci();
    sa_color bgColor =
        sa_Color_From_U8(25, 70, 125, 255); // Colors are stored as float values from 0 to 1

    struct __sc_modelMesh* mesh;

    {
        const char* filePath = "./3d/stanford-bunny-model/bunny.obj";
        mesh = sc_Model_Mesh_Load(filePath);
    }

    assert(mesh);
    sa_mat4 modelMatrix;
    sa_vec3 modelPos = {0, 0, 0};
    sa_vec3 modelRot = {0, 0, 0};
    sa_vec3 modelScale = {1, 1, 1};

    modelMatrix = sa_Mat4_Model_Matrix(modelPos, modelRot, modelScale);

    sa_vec3* pos_array;
    sa_uv* uv_array;
    sa_u64 uv_count;
    sa_u64 pos_count;
    sa_u32* vertex_indice;
    sa_u64 indice_count;
    sc_Model_Get_Position_Array(mesh, &pos_array, &pos_count);
    sc_Model_Get_Uv_Array(mesh, &uv_array, &uv_count);
    sc_Model_Get_Separated_Indice_Data(mesh, &vertex_indice, NULL, NULL, &indice_count);
    while (!sc_Window_Should_Close(window)) {
        sc_Window_Clear_Color(bgColor);
        // handle_keyboard();

        sc_Renderer_Begin(renderer);
        sc_Renderer_Bind_Uniform_Struct(renderer, (void*)&uniforms);
        sc_Renderer_Bind_Index_Buffer(renderer, vertex_indice, sa_Scast_To_m(sa_u32)(indice_count));
        printf("oi\n");
        sc_Renderer_Push_Vertex(renderer, pos_array, uv_array, NULL, sa_Scast_To_m(sa_u32) pos_count);
        printf("oi2\n");
        sc_Renderer_End(renderer);
        printf("oi3\n");
        sc_Window_Swap_Buffer(window);

        sc_Event_Poll();
    }
    sc_Model_Delete(mesh);
    sc_Renderer_Free(renderer);
    sc_Window_Free(window);
    sc_Window_Terminate();
}
