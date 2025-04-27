#include "glitch/glitc-complex.h"
#include "glitch/glitc-rand.h"
#include "glitch/glitc.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-types.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SC_MODEL_MESH_EXPOSE
#include "saci-core/sc-model.h"

#define SC_RENDERER_STRUCT_EXPOSE
#include "saci-core/sc-gl.h"

static void Test_Renderer_Batching_Integration(GlitchTester* t);

void Test_Renderer_Integration(GlitchTester* t) {
    struct __sc_renderer* rendr = sa_Malloc_m(sizeof(struct __sc_renderer));
    memset(rendr, 0, sizeof(struct __sc_renderer));

    rendr->bound_index_array_capacity = 1600 * 6 / 4;
    rendr->bound_index_array_buffer = sa_Malloc_m(sizeof(sa_u32) * rendr->bound_index_array_capacity);
    rendr->batch_index_capacity = 500 * 6 / 4;
    rendr->batch_vertex_capacity = 500;
    rendr->batch_array_capacity = 2;
    rendr->call_index_capacity = 500 * 6 / 4;
    rendr->call_vertex_capacity = 500;
    rendr->call_array_capacity = 20;

    rendr->batch_array = sa_Malloc_m(sizeof(struct __sc_batch) * rendr->batch_array_capacity);
    memset(rendr->batch_array, 0, sizeof(struct __sc_batch) * rendr->batch_array_capacity);
    for (sa_u8 i = 0; i < rendr->batch_array_capacity; ++i) {
        rendr->batch_array[i].index_array = sa_Malloc_m(sizeof(sa_u32) * rendr->batch_index_capacity);
        rendr->batch_array[i].vertex_array = sa_Malloc_m(sizeof(sa_u32) * rendr->batch_vertex_capacity);
        rendr->batch_array[i].uniform_block_size = 0;
        rendr->batch_array[i].uniform_struct_block = 0;
        rendr->batch_array[i].uniform_struct_block = NULL;
    }
    rendr->call_array = sa_Malloc_m(sizeof(struct __sc_batch) * rendr->call_array_capacity);
    memset(rendr->call_array, 0, sizeof(struct __sc_batch) * rendr->call_array_capacity);
    for (sa_u8 i = 0; i < rendr->call_array_capacity; ++i) {
        rendr->call_array[i].index_array = sa_Malloc_m(sizeof(sa_u32) * rendr->call_index_capacity);
        rendr->call_array[i].vertex_array = sa_Malloc_m(sizeof(sa_u32) * rendr->call_vertex_capacity);
        rendr->call_array[i].uniform_block_size = 0;
        rendr->call_array[i].uniform_struct_block = 0;
        rendr->call_array[i].uniform_struct_block = NULL;
    }

    struct __sc_modelMesh* mesh = sc_Model_Mesh_Load("../assets/suzanne.obj");
    sa_Log_Assert_Message_m(mesh, "Mesh not loaded");

    sa_vec3* pos_array;
    sa_uv* uv_array;
    sa_u64 uv_count;
    sa_u64 pos_count;
    sa_u32* vertex_indice;
    sa_u64 indice_count;
    sc_Model_Get_Position_Array(mesh, &pos_array, &pos_count);
    sc_Model_Get_Uv_Array(mesh, &uv_array, &uv_count);
    sc_Model_Get_Separated_Indice_Data(mesh, &vertex_indice, NULL, NULL, &indice_count);
    sc_Renderer_Begin(rendr);
    sc_Renderer_Bind_Index_Buffer(rendr, vertex_indice, sa_Scast_To_m(sa_u32)(indice_count));
    sc_Renderer_Push_Vertex(rendr, pos_array, uv_array, NULL, sa_Scast_To_m(sa_u32) pos_count);
    sc_Renderer_End(rendr);
}
