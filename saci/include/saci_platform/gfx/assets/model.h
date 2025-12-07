#ifndef SACI_BACKEND_ASSETS_SB_MODEL_H
#define SACI_BACKEND_ASSETS_SB_MODEL_H

#include "saci_util/math.h"
#include "saci_util/defines.h"
#include "saci_util/types.h"

typedef struct PSaciModelMesh PSaciModelMesh;
typedef struct PSaciVertexIndex PSaciVertexIndex;

#ifdef SB_MODEL_MESH_EXPOSE
#  ifndef SC_MODEL_MESH_STRUCT
#    define SC_MODEL_MESH_STRUCT

struct sc_VertexIndex {
    sa_u32 vertex_index;
    sa_u32 uv_index;
    sa_u32 normal_index;
};

struct sc_ModelMesh {
    sa_u64 indices_count;
    sa_u64 uv_count;
    sa_u64 positions_count;
    struct sc_vertexIndex* indice_array;
    sa_uv* uv_array;
    sa_vec3* position_array;
};

#  endif // SB_MODEL_MESH_STRUCT
#endif   // SB_MODEL_MESH_EXPOSE

SACI_API struct PSaciModelMesh* psaci_model_mesh_load(const char* path);

SACI_API void psaci_model_delete(struct PSaciModelMesh* model_mesh);

SACI_API void psaci_model_get_vertex_index_array(const struct PSaciModelMesh* model_mesh,
                                                 struct PSaciVertexIndex** indice_array_out,
                                                 SaciU64* indice_count_out);

SACI_API void psaci_model_get_separated_index_data(const struct PSaciModelMesh* model_mesh,
                                                   SaciU32** vertex_index_array_out,
                                                   SaciU32** uv_index_array_out,
                                                   SaciU32** normal_index_array_out,
                                                   SaciU64* indice_count_out);

SACI_API void psaci_model_get_position_array(const struct PSaciModelMesh* model_mesh,
                                             SaciVec3** position_array_out,
                                             SaciU64* position_count_out);

SACI_API void psaci_model_get_uv_array(const struct PSaciModelMesh* model_mesh,
                                       SaciUv** uv_array_out,
                                       SaciU64* uv_count_out);

SACI_API void psaci_model_vertex_index_get_data(const struct PSaciVertexIndex* vertex_indice,
                                                SaciU64 vertex_indice_amount,
                                                SaciU32** vertex_index_out,
                                                SaciU32** uv_index_out,
                                                SaciU32** normal_index_out);

#endif // SACI_BACKEND_ASSETS_SB_MODEL_H
