#ifndef __SACI_CORE_SC_MODEL_H__
#define __SACI_CORE_SC_MODEL_H__

#include <saci-utils/su-types.h>

struct sc_modelMesh_c;

struct sc_vertexIndice_c {
    sa_u32_t vertexIndex;
    sa_u32_t texCoordIndex;
    sa_u32_t normalIndex;
};

sa_bool_t sc_Model_Parse(const char* file_path,
                         sa_vec3_t** positions_out, sa_u64_t* positions_count_out,
                         sa_vec2_t** texcoords_out, sa_u64_t* texcoord_count_out,
                         struct sc_vertexIndice_c** indices_out, sa_u64_t* indices_count_out);
struct sc_modelMesh_c* sc_Model_Mesh_Load(const char* path);
void sc_Model_Mesh_Delete(struct sc_modelMesh_c* model_mesh);
sa_u64_t sc_ModelMesh_Get_Vertices_Amount(const struct sc_modelMesh_c* modelMesh);
sa_u32_t* sc_ModelMesh_Get_Indices(const struct sc_modelMesh_c* modelMesh);
sa_u64_t sc_ModelMesh_GetIndicesAmount(const struct sc_modelMesh_c* modelMesh);

#endif
