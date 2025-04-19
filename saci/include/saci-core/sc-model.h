#ifndef __SACI_CORE_SC_MODEL_H__
#define __SACI_CORE_SC_MODEL_H__

#include <saci-utils/su-types.h>

struct sc_modelMesh_c;

struct sc_vertexIndice_c {
    sa_u32 vertexIndex;
    sa_u32 texCoordIndex;
    sa_u32 normalIndex;
};

sa_bool sc_Model_Parse(const char* file_path,
                       sa_vec3** positions_out, sa_u64* positions_count_out,
                       sa_vec2** texcoords_out, sa_u64* texcoord_count_out,
                       struct sc_vertexIndice_c** indices_out, sa_u64* indices_count_out);
struct sc_modelMesh_c* sc_Model_Mesh_Load(const char* path);
void sc_Model_Mesh_Delete(struct sc_modelMesh_c* model_mesh);
sa_u64 sc_ModelMesh_Get_Vertices_Amount(const struct sc_modelMesh_c* modelMesh);
sa_u32* sc_ModelMesh_Get_Indices(const struct sc_modelMesh_c* modelMesh);
sa_u64 sc_ModelMesh_GetIndicesAmount(const struct sc_modelMesh_c* modelMesh);

#endif
