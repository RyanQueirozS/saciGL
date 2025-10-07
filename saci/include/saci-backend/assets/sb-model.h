#ifndef SACI_BACKEND_SB_MODEL_H
#define SACI_BACKEND_SB_MODEL_H

#include "saci-utils/su-types-common.h"
#include "saci-utils/math/su-math-types.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-log.h"

typedef struct sb_ModelMesh sb_ModelMesh;
typedef struct sb_VertexIndex sb_VertexIndex;

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

struct sb_ModelMesh* sb_model_mesh_load(const char* path);

void sb_model_delete(struct sb_ModelMesh* model_mesh);

void sb_model_get_vertex_index_array(const struct sb_ModelMesh* model_mesh,
                                     struct sb_VertexIndex** indice_array_out,
                                     su_U64* indice_count_out);

void sb_model_get_separated_index_data(const struct sb_ModelMesh* model_mesh,
                                       su_U32** vertex_index_array_out,
                                       su_U32** uv_index_array_out,
                                       su_U32** normal_index_array_out,
                                       su_U64* indice_count_out);

void sb_model_get_position_array(const struct sb_ModelMesh* model_mesh,
                                 su_Vec3** position_array_out,
                                 su_U64* position_count_out);

void sb_model_get_uv_array(const struct sb_ModelMesh* model_mesh,
                           su_Uv** uv_array_out,
                           su_U64* uv_count_out);

void sb_model_vertex_index_get_data(const struct sb_VertexIndex* vertex_indice,
                                    su_U64 vertex_indice_amount,
                                    su_U32** vertex_index_out,
                                    su_U32** uv_index_out,
                                    su_U32** normal_index_out);

#endif // SACI_BACKEND_SB_MODEL_H
