#define GLITCH_STD
#include "glitch/glitc-complex.h"
#include "glitch/glitc-rand.h"
#include "glitch/glitc.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SC_MODEL_MESH_EXPOSE
#define SC_RENDERER_STRUCT_EXPOSE
#include "saci-core/sc-model.h"
#include "saci-core/sc-gl.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-types.h"

extern void s_Renderer_Call_Array_Sort(struct sc_renderCall* call_array_out, sa_u8 call_amount);
extern void s_Renderer_Generate_Batch(struct sc_renderer* rendr);

void Test_Batch_Array_Mesh(GlitchTester* t);

int main(void) {
    GlitchTester* t = glitch_Tester_New();
    Test_Batch_Array_Mesh(t);
    glitch_End(t);
}

void Test_Batch_Array_Mesh(GlitchTester* t) {
    struct sc_renderer* rendr =
        sc_Renderer_New_From_Config("assets/cfgs/sc-gl-integration-config-1.lua");

    sc_modelMesh* mesh = sc_Model_Mesh_Load("assets/suzanne.obj");
    sa_Log_Assert_Message_m(mesh, "Mesh not loaded");

    GLITCH_ASSERT(t, mesh->positions_count == 507, "Position count doesn't match");
    GLITCH_ASSERT(t, mesh->indices_count == 2904, "Index count doesn't match");

    sa_u32* indices = sa_Calloc_m(mesh->indices_count, sizeof(sa_u32));
    for (sa_u32 i = 0; i < mesh->indices_count; ++i) {
        indices[i] = mesh->indice_array[i].vertex_index;
    }
    sc_Renderer_Begin(rendr);
    sc_Renderer_Bind_Index_Buffer(rendr, indices,
                                  mesh->indices_count);
    sc_Renderer_Push_Vertex(rendr, mesh->position_array, mesh->uv_array,
                            NULL, sa_Scast_To_m(sa_u32) mesh->positions_count);
    GLITCH_ASSERT(t, mesh->indices_count == rendr->call_array[0].index_array_length, "Invalid index count");
    for (sa_u64 i = 0; i < mesh->indices_count; ++i) {
        GLITCH_ASSERT(t, mesh->indice_array[i].vertex_index == rendr->call_array[0].index_array[i], "Invalid value for index array");
    }
    // s_Renderer_Call_Array_Sort(rendr->call_array, rendr->call_in_use);
    // s_Renderer_Generate_Batch(rendr);
    // for (sa_u64 i = 0; i < mesh->indices_count; ++i) {
    //     GLITCH_ASSERT(t, mesh->indice_array[i].vertex_index == rendr->batch_array[0].index_array[i], "Invalid value for index array");
    // }
}
