#define GLITCH_STD
#include "glitch/glitc-complex.h"
#include "glitch/glitc-rand.h"
#include "glitch/glitc.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SC_MODEL_MESH_EXPOSE
#include "saci-core/sc-model.h"
#include "saci-utils/su-debug.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-types.h"

#define SC_RENDERER_STRUCT_EXPOSE
#include "saci-core/sc-gl.h"

void Test_Renderer_Integration(GlitchTester* t);

int main(void) {
    GlitchTester* t = glitch_Tester_New();
    Test_Renderer_Integration(t);
}

void Test_Renderer_Integration(GlitchTester* t) {
    struct sc_renderer* rendr = sc_Renderer_New_Default();

    struct sc_modelMesh* mesh = sc_Model_Mesh_Load("../assets/suzanne.obj");
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
