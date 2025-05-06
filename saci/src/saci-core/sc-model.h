#ifndef SACI_CORE_SC_MODEL_H
#define SACI_CORE_SC_MODEL_H

#include "saci-utils/su-types.h"

typedef struct sc_modelMesh sc_modelMesh;
typedef struct sc_vertexIndice sc_vertexIndice;

#ifdef SC_MODEL_MESH_EXPOSE
#  ifndef SC_MODEL_MESH_STRUCT
#    define SC_MODEL_MESH_STRUCT

struct __sc_vertexIndice {
    sa_u32 vertex_index;
    sa_u32 uv_index;
    sa_u32 normal_index;
};

struct __sc_modelMesh {
    sa_u64 indices_count;
    sa_u64 uv_count;
    sa_u64 positions_count;
    struct __sc_vertexIndice* indice_array;
    sa_uv* uv_array;
    sa_vec3* position_array;
};

#  endif // SC_MODEL_MESH_STRUCT
#endif   // SC_MODEL_MESH_EXPOSE

struct sc_modelMesh* sc_Model_Mesh_Load(const char* path);

void sc_Model_Delete(struct sc_modelMesh* model_mesh);

void sc_Model_Get_Vertex_Indice_Array(const struct sc_modelMesh* model_mesh,
                                      struct sc_vertexIndice** indice_array_out,
                                      sa_u64* indice_count_out);

void sc_Model_Get_Separated_Indice_Data(const struct sc_modelMesh* model_mesh,
                                        sa_u32** vertex_index_array_out,
                                        sa_u32** uv_index_array_out,
                                        sa_u32** normal_index_array_out,
                                        sa_u64* indice_count_out);

void sc_Model_Get_Position_Array(const struct sc_modelMesh* model_mesh,
                                 sa_vec3** position_array_out,
                                 sa_u64* position_count_out);

void sc_Model_Get_Uv_Array(const struct sc_modelMesh* model_mesh,
                           sa_uv** uv_array_out,
                           sa_u64* uv_count_out);

void sc_Model_Vertex_Indice_Get_Data(const struct sc_vertexIndice* vertex_indice,
                                     sa_u64 vertex_indice_amount,
                                     sa_u32** vertex_index_out,
                                     sa_u32** uv_index_out,
                                     sa_u32** normal_index_out);

#endif // SACI_CORE_SC_MODEL_H
