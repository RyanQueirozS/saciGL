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

SA_INTERNAL char* __sc_file_buffer = NULL; // buffer for the file reader function.
SA_INTERNAL size_t __sc_file_buffer_len = 0;

SA_INTERNAL sa_bool __sc_Model_Parse(const char* file_path,
                                     sa_vec3** position_array_out, sa_u64* positions_count_out,
                                     sa_uv** texcoord_array_out, sa_u64* texcoord_count_out,
                                     struct __sc_vertexIndice** indice_array_out, sa_u64* indices_count_out);

SA_INTERNAL void __sc_File_Reader_Function(void* ctx, const char* filename, int isMtl,
                                           const char* obj_filename, char** buf, size_t* len);

/* === Model Loading Implementation === */

#ifndef SC_MODEL_MESH_STRUCT
#  define SC_MODEL_MESH_STRUCT
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
#endif

SA_API struct __sc_modelMesh* sc_Model_Mesh_Load(const char* path) {
    struct __sc_modelMesh* mesh = sa_Malloc_m(sizeof(struct __sc_modelMesh));
    sa_bool success = __sc_Model_Parse(path,
                                       &mesh->position_array,
                                       &mesh->positions_count,
                                       &mesh->uv_array,
                                       &mesh->uv_count,
                                       &mesh->indice_array,
                                       &mesh->indices_count);
    if (!success) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_MEDIUM, sa_LOG_CONTEXT_MODEL_LOADING, "Couldn't load model");
        sa_Free_m(mesh);
    }
    return mesh;
}

void sc_Model_Delete(struct __sc_modelMesh* model_mesh) {
    sa_Free_m(model_mesh->indice_array);
    sa_Free_m(model_mesh->position_array);
    sa_Free_m(model_mesh->uv_array);

    model_mesh->indice_array = NULL;
    model_mesh->position_array = NULL;
    model_mesh->uv_array = NULL;

    sa_Free_m(model_mesh);
}

SA_API void sc_Model_Get_Vertex_Indice_Array(const struct __sc_modelMesh* model_mesh,
                                             struct __sc_vertexIndice** indice_array_out,
                                             sa_u64* indice_count_out) {
    *indice_array_out = sa_Malloc_m(sizeof(struct __sc_vertexIndice) *
                                    model_mesh->indices_count);
    if (!*indice_array_out) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR,
                             sa_LOG_SEVERITY_MEDIUM,
                             sa_LOG_CONTEXT_MEMORY_ALLOCATION,
                             "Could not allocate memory for indice_array_out");
        return;
    }
    memcpy(*indice_array_out, model_mesh->indice_array,
           (sizeof(struct __sc_vertexIndice) * model_mesh->indices_count));
    *indice_count_out = model_mesh->indices_count;
}

SA_API void sc_Model_Get_Separated_Indice_Data(const struct __sc_modelMesh* model_mesh,
                                               sa_u32** vertex_index_out,
                                               sa_u32** uv_index_out,
                                               sa_u32** normal_index_out,
                                               sa_u64* indice_array_count_out) {
    *indice_array_count_out = model_mesh->indices_count;

    if (vertex_index_out)
        *vertex_index_out = (sa_u32*)sa_Malloc_m(sizeof(sa_u32) * (*indice_array_count_out));
    if (uv_index_out)
        *uv_index_out = (sa_u32*)sa_Malloc_m(sizeof(sa_u32) * (*indice_array_count_out));
    if (normal_index_out)
        *normal_index_out = (sa_u32*)sa_Malloc_m(sizeof(sa_u32) * (*indice_array_count_out));

    for (sa_u64 i = 0; i < *indice_array_count_out; ++i) {
        if (vertex_index_out)
            (*vertex_index_out)[i] = model_mesh->indice_array[i].vertex_index;
        if (uv_index_out)
            (*uv_index_out)[i] = model_mesh->indice_array[i].uv_index;
        if (normal_index_out)
            (*normal_index_out)[i] = model_mesh->indice_array[i].normal_index;
    }
}

SA_API void sc_Model_Get_Position_Array(const struct __sc_modelMesh* model_mesh,
                                        sa_vec3** position_array_out,
                                        sa_u64* position_count_out) {
    *position_array_out = sa_Malloc_m(sizeof(sa_vec3) * model_mesh->positions_count);
    *position_count_out = model_mesh->positions_count;
    memcpy(*position_array_out, model_mesh->position_array,
           (sizeof(sa_vec3) * model_mesh->positions_count));
}

SA_API void sc_Model_Get_Uv_Array(const struct __sc_modelMesh* model_mesh,
                                  sa_uv** uv_array_out,
                                  sa_u64* uv_count_out) {
    *uv_array_out = sa_Malloc_m(sizeof(sa_uv) * model_mesh->uv_count);
    *uv_count_out = model_mesh->uv_count;
    memcpy(*uv_array_out, model_mesh->uv_array,
           (sizeof(sa_uv) * model_mesh->uv_count));
}

SA_API void sc_Model_Vertex_Indice_Get_Data(const struct __sc_vertexIndice* vertex_indice,
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

SA_INTERNAL sa_bool __sc_Model_Parse(const char* filePath,
                                     sa_vec3** position_array_out, sa_u64* positions_count_out,
                                     sa_uv** uv_array_out, sa_u64* uv_count_out,
                                     struct __sc_vertexIndice** indice_array_out, sa_u64* indices_count_out) {
    tinyobj_attrib_t attribute = {0};
    tinyobj_shape_t* shape_array = NULL;
    sa_u64 shape_array_amount = 0;
    tinyobj_material_t* material_array = NULL;
    sa_u64 material_array_size = 0;

    tinyobj_parse_obj(&attribute, &shape_array, &shape_array_amount, &material_array,
                      &material_array_size, filePath, __sc_File_Reader_Function, NULL, 0);

    *positions_count_out = attribute.num_vertices;
    if (!(*positions_count_out)) {
        fprintf(stderr, "Malloc Error\n");
    }
    *position_array_out = sa_Scast_To_m(sa_vec3*) sa_Malloc_m(sizeof(sa_vec3) * (*positions_count_out));
    if (!(*position_array_out)) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_MEDIUM,
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
    *indice_array_out = sa_Malloc_m(sizeof(struct __sc_vertexIndice) * (*indices_count_out));
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
    sa_Free_m(__sc_file_buffer);

    return true;
}

#include <stdio.h>
#include <stdlib.h>

SA_INTERNAL void __sc_File_Reader_Function(void* ctx, const char* filename, int isMtl,
                                           const char* objFilename2, char** buf, size_t* len) {
    sa_Not_Used_m(ctx); // suppress unused warning
    sa_Not_Used_m(isMtl);
    sa_Not_Used_m(objFilename2);

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
    if (__sc_file_buffer_len < file_size + 1) {
        free(__sc_file_buffer); // free old buffer if any
        __sc_file_buffer = (char*)malloc(file_size + 1);
        if (!__sc_file_buffer) {
            fclose(file);
            *buf = NULL;
            *len = 0;
            return;
        }
        __sc_file_buffer_len = file_size + 1;
    }

    fread(__sc_file_buffer, 1, file_size, file);
    fclose(file);

    __sc_file_buffer[file_size] = '\0'; // null-terminate just in case

    *buf = __sc_file_buffer;
    *len = file_size;
}
