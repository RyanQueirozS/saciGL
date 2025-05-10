#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "saci-core/sc-model.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"

// #define TINYOBJ_FREE
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c/tinyobj_loader_c.h"

/* === Helper definitions === */

SA_INTERNAL char* sc_file_buffer_s = NULL; // buffer for the file reader function.
SA_INTERNAL size_t sc_file_buffer_len_s = 0;

SA_INTERNAL sa_bool s_Model_Parse(const char* file_path,
                                  sa_vec3** position_array_out, sa_u64* positions_count_out,
                                  sa_uv** texcoord_array_out, sa_u64* texcoord_count_out,
                                  struct sc_vertexIndice** indice_array_out, sa_u64* indices_count_out);

SA_INTERNAL void s_File_Reader_Function(void* ctx, const char* filename, int is_mtl,
                                        const char* obj_filename, char** buf, size_t* len);

/* === Model Loading Implementation === */

#ifndef SC_MODEL_MESH_STRUCT
#  define SC_MODEL_MESH_STRUCT
struct sc_vertexIndice {
    sa_u32 vertex_index;
    sa_u32 uv_index;
    sa_u32 normal_index;
};

struct sc_modelMesh {
    sa_u64 indices_count;
    sa_u64 uv_count;
    sa_u64 positions_count;
    struct sc_vertexIndice* indice_array;
    sa_uv* uv_array;
    sa_vec3* position_array;
};
#endif

SA_API struct sc_modelMesh* sc_Model_Mesh_Load(const char* path) {
    struct sc_modelMesh* mesh = sa_Malloc_m(sizeof(struct sc_modelMesh));
    sa_bool success = s_Model_Parse(path,
                                    &mesh->position_array,
                                    &mesh->positions_count,
                                    &mesh->uv_array,
                                    &mesh->uv_count,
                                    &mesh->indice_array,
                                    &mesh->indices_count);
    if (!success) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MODEL_LOADING, "Couldn't load model");
        sa_Free_m(mesh);
        return NULL;
    }
    return mesh;
}

void sc_Model_Delete(struct sc_modelMesh* model_mesh) {
    sa_Free_m(model_mesh->indice_array);
    sa_Free_m(model_mesh->position_array);
    sa_Free_m(model_mesh->uv_array);

    model_mesh->indice_array = NULL;
    model_mesh->position_array = NULL;
    model_mesh->uv_array = NULL;

    sa_Free_m(model_mesh);
    model_mesh = NULL;
}

SA_API void sc_Model_Get_Vertex_Indice_Array(const struct sc_modelMesh* model_mesh,
                                             struct sc_vertexIndice** indice_array_out,
                                             sa_u64* indice_count_out) {
    *indice_array_out = sa_Malloc_m(sizeof(struct sc_vertexIndice) *
                                    model_mesh->indices_count);
    if (!*indice_array_out) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM,
                             sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                             "Could not allocate memory for indice_array_out");
        return;
    }
    memcpy(*indice_array_out, model_mesh->indice_array,
           (sizeof(struct sc_vertexIndice) * model_mesh->indices_count));
    *indice_count_out = model_mesh->indices_count;
}

SA_API void sc_Model_Get_Separated_Indice_Data(const struct sc_modelMesh* model_mesh,
                                               sa_u32** vertex_index_out,
                                               sa_u32** uv_index_out,
                                               sa_u32** normal_index_out,
                                               sa_u64* indice_array_count_out) {
    *indice_array_count_out = model_mesh->indices_count;

    if (vertex_index_out) {
        *vertex_index_out = (sa_u32*)sa_Malloc_m(sizeof(sa_u32) * (*indice_array_count_out));
    }
    if (uv_index_out) {
        *uv_index_out = (sa_u32*)sa_Malloc_m(sizeof(sa_u32) * (*indice_array_count_out));
    }
    if (normal_index_out) {
        *normal_index_out = (sa_u32*)sa_Malloc_m(sizeof(sa_u32) * (*indice_array_count_out));
    }

    for (sa_u64 i = 0; i < *indice_array_count_out; ++i) {
        if (vertex_index_out) {
            (*vertex_index_out)[i] = model_mesh->indice_array[i].vertex_index;
        }
        if (uv_index_out) {
            (*uv_index_out)[i] = model_mesh->indice_array[i].uv_index;
        }
        if (normal_index_out) {
            (*normal_index_out)[i] = model_mesh->indice_array[i].normal_index;
        }
    }
}

SA_API void sc_Model_Get_Position_Array(const struct sc_modelMesh* model_mesh,
                                        sa_vec3** position_array_out,
                                        sa_u64* position_count_out) {
    *position_array_out = sa_Malloc_m(sizeof(sa_vec3) * model_mesh->positions_count);
    *position_count_out = model_mesh->positions_count;
    memcpy(*position_array_out, model_mesh->position_array,
           (sizeof(sa_vec3) * model_mesh->positions_count));
}

SA_API void sc_Model_Get_Uv_Array(const struct sc_modelMesh* model_mesh,
                                  sa_uv** uv_array_out,
                                  sa_u64* uv_count_out) {
    *uv_array_out = sa_Malloc_m(sizeof(sa_uv) * model_mesh->uv_count);
    *uv_count_out = model_mesh->uv_count;
    memcpy(*uv_array_out, model_mesh->uv_array,
           (sizeof(sa_uv) * model_mesh->uv_count));
}

SA_API void sc_Model_Vertex_Indice_Get_Data(const struct sc_vertexIndice* vertex_indice,
                                            sa_u64 vertex_indice_amount,
                                            sa_u32** vertex_index_out,
                                            sa_u32** uv_index_out,
                                            sa_u32** normal_index_out) {
    *vertex_index_out = sa_Malloc_m(sizeof(sa_u32) * vertex_indice_amount);
    *uv_index_out = sa_Malloc_m(sizeof(sa_u32) * vertex_indice_amount);
    *normal_index_out = sa_Malloc_m(sizeof(sa_u32) * vertex_indice_amount);
    for (sa_u64 i = 0; i < vertex_indice_amount; ++i) {
        (*vertex_index_out)[i] = vertex_indice[i].vertex_index;
        (*uv_index_out)[i] = vertex_indice[i].uv_index;
        (*normal_index_out)[i] = vertex_indice[i].normal_index;
    }
}

/* === Helper Implementation === */

SA_INTERNAL sa_bool s_Model_Parse(const char* file_path,
                                  sa_vec3** position_array_out,
                                  sa_u64* positions_count_out,
                                  sa_uv** uv_array_out,
                                  sa_u64* uv_count_out,
                                  struct sc_vertexIndice** indice_array_out,
                                  sa_u64* indices_count_out) {
    tinyobj_attrib_t attribute = {0};
    tinyobj_shape_t* shape_array = NULL;
    sa_u64 shape_array_amount = 0;
    tinyobj_material_t* material_array = NULL;
    sa_u64 material_array_size = 0;

    sa_s32 success = tinyobj_parse_obj(&attribute, &shape_array, &shape_array_amount, &material_array,
                                       &material_array_size, file_path, s_File_Reader_Function, NULL, TINYOBJ_FLAG_TRIANGULATE);
    if (success < 0) {
        char error_reason[255] = "UNKNOWN";
        switch (success) {
        case TINYOBJ_ERROR_EMPTY:
            strncpy(error_reason, "Empty file", 255);
            break;
        case TINYOBJ_ERROR_INVALID_PARAMETER:
            strncpy(error_reason, "Invalid param", 255);
            break;
        case TINYOBJ_ERROR_FILE_OPERATION:
            strncpy(error_reason, "Error reading file", 255);
            break;
        }
        sa_Log_ErrorF_Print_m(sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MODEL_LOADING, "Could not load model: %s", error_reason);
    };

    *positions_count_out = attribute.num_vertices;
    if (!(*positions_count_out)) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM,
                             sa_LOG_CONTEXT_MODEL_LOADING,
                             "Could not load position array");
        return false;
    }
    *position_array_out = sa_Scast_To_m(sa_vec3*) sa_Malloc_m(sizeof(sa_vec3) * (*positions_count_out));
    if (!(*position_array_out)) {
        sa_Log_Error_Print_m(sa_LOG_SEVERITY_MEDIUM,
                             sa_LOG_CONTEXT_MODEL_LOADING, "Couldn't malloc positions");
        return false;
    }
    for (sa_u64 i = 0; i < (*positions_count_out); ++i) {
        float pos_x = attribute.vertices[3 * i + 0];
        float pos_y = attribute.vertices[3 * i + 1];
        float pos_z = attribute.vertices[3 * i + 2];
        (*position_array_out)[i] = sa_Scast_To_m(sa_vec3){pos_x, pos_y, pos_z};
    }

    *uv_count_out = attribute.num_texcoords;
    *uv_array_out = sa_Malloc_m(sizeof(sa_uv) * (*uv_count_out));
    for (sa_u64 i = 0; i < (*uv_count_out); ++i) {
        float u = attribute.texcoords[2 * i + 0];
        float v = attribute.texcoords[2 * i + 1];
        (*uv_array_out)[i] = (sa_uv){u, v};
    }

    *indices_count_out = attribute.num_faces;
    *indice_array_out = sa_Malloc_m(sizeof(struct sc_vertexIndice) * (*indices_count_out));
    for (sa_u64 i = 0; i < (attribute.num_faces); ++i) {
        (*indice_array_out)[i].vertex_index = sa_Scast_To_m(sa_u32)(attribute.faces[i].v_idx);
        (*indice_array_out)[i].uv_index = sa_Scast_To_m(sa_u32) attribute.faces[i].vt_idx;
        (*indice_array_out)[i].normal_index = sa_Scast_To_m(sa_u32) attribute.faces[i].vn_idx;
    }

    // These will not be changed to sa_FREE, since they are already changed
    // through macro
    tinyobj_attrib_free(&attribute);
    tinyobj_shapes_free(shape_array, shape_array_amount);
    tinyobj_materials_free(material_array, material_array_size);
    sa_Free_m(sc_file_buffer_s);

    return true;
}

#include <stdio.h>
#include <stdlib.h>

SA_INTERNAL void s_File_Reader_Function(void* ctx, const char* filename, int is_mtl,
                                        const char* obj_filename2, char** buf, size_t* len) {
    sa_Not_Used_m(ctx); // suppress unused warning
    sa_Not_Used_m(is_mtl);
    sa_Not_Used_m(obj_filename2);

    FILE* file = fopen(filename, "rb");
    if (!file) {
        *buf = NULL;
        *len = 0;
        return;
    }

    fseek(file, 0, SEEK_END);
    sa_u64 file_size = sa_Scast_To_m(sa_u64)(ftell(file));
    rewind(file);

    // Allocate or reallocate the static buffer if needed
    if (sc_file_buffer_len_s < file_size + 1) {
        free(sc_file_buffer_s); // free old buffer if any
        sc_file_buffer_s = (char*)malloc(file_size + 1);
        if (!sc_file_buffer_s) {
            fclose(file);
            *buf = NULL;
            *len = 0;
            return;
        }
        sc_file_buffer_len_s = file_size + 1;
    }

    fread(sc_file_buffer_s, 1, file_size, file);
    fclose(file);

    sc_file_buffer_s[file_size] = '\0'; // null-terminate just in case

    *buf = sc_file_buffer_s;
    *len = file_size;
}
