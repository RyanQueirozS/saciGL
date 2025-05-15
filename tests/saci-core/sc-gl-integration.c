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
    for (sa_u8 i = 0; i < rendr->batch_in_use; ++i) {
        for (sa_u64 j = 0; j < rendr->batch_index_capacity; ++j) {
            sa_u32 idx = (i * rendr->batch_index_capacity) + j;
            if (mesh->indice_array[idx].vertex_index != rendr->batch_array[i].index_array[j]) {
                // printf("i:%d %d\t%d\n", i, mesh->indice_array[idx].vertex_index, rendr->batch_array[i].index_array[j]);
            }
            GLITCH_ASSERT(t, mesh->indice_array[idx].vertex_index == rendr->batch_array[i].index_array[j], "Invalid value for index array");
        }
    }
}
