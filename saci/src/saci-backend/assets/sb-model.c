#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "saci-backend/assets/sb-model.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-log.h"

// #define TINYOBJ_FREE
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c/tinyobj_loader_c.h"

/* === Helper definitions === */

SA_INTERNAL char* sb_file_buffer_s = NULL; // buffer for the file reader function.
SA_INTERNAL size_t sb_file_buffer_len_s = 0;

SA_INTERNAL su_Bool sb__model_parse(const char* file_path,
                                    su_Vec3** position_array_out, su_U64* positions_count_out,
                                    su_Uv** texcoord_array_out, su_U64* texcoord_count_out,
                                    struct sb_VertexIndex** index_array_out, su_U64* index_count_out);

SA_INTERNAL void sb__file_reader_function(void* ctx, const char* filename, int is_mtl,
                                          const char* obj_filename, char** buf, size_t* len);

/* === Model Loading Implementation === */

#ifndef sb_MODEL_MESH_STRUCT
#  define sb_MODEL_MESH_STRUCT
struct sb_VertexIndex {
    su_U32 vertex_index;
    su_U32 uv_index;
    su_U32 normal_index;
};

struct sb_ModelMesh {
    su_U64 index_count;
    su_U64 uv_count;
    su_U64 positions_count;
    struct sb_VertexIndex* index_array;
    su_Uv* uv_array;
    su_Vec3* position_array;
};
#endif

struct sb_ModelMesh* sb_model_mesh_load(const char* path) {
    struct sb_ModelMesh* mesh = malloc(sizeof(struct sb_ModelMesh));
    su_Bool success = sb__model_parse(path,
                                      &mesh->position_array,
                                      &mesh->positions_count,
                                      &mesh->uv_array,
                                      &mesh->uv_count,
                                      &mesh->index_array,
                                      &mesh->index_count);
    if (!success) {
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_MODEL_LOADING, "Couldn't load model");
        free(mesh);
        return NULL;
    }
    return mesh;
}

void sb_model_delete(struct sb_ModelMesh* model_mesh) {
    free(model_mesh->index_array);
    free(model_mesh->position_array);
    free(model_mesh->uv_array);

    model_mesh->index_array = NULL;
    model_mesh->position_array = NULL;
    model_mesh->uv_array = NULL;

    free(model_mesh);
    model_mesh = NULL;
}

void sb_model_get_vertex_index_array(const struct sb_ModelMesh* model_mesh,
                                     struct sb_VertexIndex** index_array_out,
                                     su_U64* index_count_out) {
    *index_array_out = malloc(sizeof(struct sb_VertexIndex) *
                              model_mesh->index_count);
    if (!*index_array_out) {
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_MEDIUM,
                       su_LOG_CONTEXT_MODEL_LOADING,
                       "Could not allocate memory for index_array_out");
        return;
    }
    memcpy(*index_array_out, model_mesh->index_array,
           (sizeof(struct sb_VertexIndex) * model_mesh->index_count));
    *index_count_out = model_mesh->index_count;
}

void sb_model_get_separated_index_data(const struct sb_ModelMesh* model_mesh,
                                       su_U32** vertex_index_out,
                                       su_U32** uv_index_out,
                                       su_U32** normal_index_out,
                                       su_U64* index_array_count_out) {
    *index_array_count_out = model_mesh->index_count;

    if (vertex_index_out) {
        *vertex_index_out = malloc(sizeof(su_U32) * (*index_array_count_out));
    }
    if (uv_index_out) {
        *uv_index_out = malloc(sizeof(su_U32) * (*index_array_count_out));
    }
    if (normal_index_out) {
        *normal_index_out = malloc(sizeof(su_U32) * (*index_array_count_out));
    }

    for (su_U64 i = 0; i < *index_array_count_out; ++i) {
        if (vertex_index_out) {
            (*vertex_index_out)[i] = model_mesh->index_array[i].vertex_index;
        }
        if (uv_index_out) {
            (*uv_index_out)[i] = model_mesh->index_array[i].uv_index;
        }
        if (normal_index_out) {
            (*normal_index_out)[i] = model_mesh->index_array[i].normal_index;
        }
    }
}

void sb_model_get_position_array(const struct sb_ModelMesh* model_mesh,
                                 su_Vec3** position_array_out,
                                 su_U64* position_count_out) {
    *position_array_out = malloc(sizeof(su_Vec3) * model_mesh->positions_count);
    *position_count_out = model_mesh->positions_count;
    memcpy(*position_array_out, model_mesh->position_array,
           (sizeof(su_Vec3) * model_mesh->positions_count));
}

void sb_model_get_uv_array(const struct sb_ModelMesh* model_mesh,
                           su_Uv** uv_array_out,
                           su_U64* uv_count_out) {
    *uv_array_out = malloc(sizeof(su_Uv) * model_mesh->uv_count);
    *uv_count_out = model_mesh->uv_count;
    memcpy(*uv_array_out, model_mesh->uv_array,
           (sizeof(su_Uv) * model_mesh->uv_count));
}

void sb_model_vertex_index_get_data(const struct sb_VertexIndex* vertex_index,
                                    su_U64 vertex_index_amount,
                                    su_U32** vertex_index_out,
                                    su_U32** uv_index_out,
                                    su_U32** normal_index_out) {
    *vertex_index_out = malloc(sizeof(su_U32) * vertex_index_amount);
    *uv_index_out = malloc(sizeof(su_U32) * vertex_index_amount);
    *normal_index_out = malloc(sizeof(su_U32) * vertex_index_amount);
    for (su_U64 i = 0; i < vertex_index_amount; ++i) {
        (*vertex_index_out)[i] = vertex_index[i].vertex_index;
        (*uv_index_out)[i] = vertex_index[i].uv_index;
        (*normal_index_out)[i] = vertex_index[i].normal_index;
    }
}

/* === Helper Implementation === */

SA_INTERNAL su_Bool sb__model_parse(const char* file_path,
                                    su_Vec3** position_array_out,
                                    su_U64* positions_count_out,
                                    su_Uv** uv_array_out,
                                    su_U64* uv_count_out,
                                    struct sb_VertexIndex** index_array_out,
                                    su_U64* index_count_out) {
    tinyobj_attrib_t attribute = {0};
    tinyobj_shape_t* shape_array = NULL;
    su_U64 shape_array_amount = 0;
    tinyobj_material_t* material_array = NULL;
    su_U64 material_array_size = 0;

    su_S32 success = tinyobj_parse_obj(&attribute, &shape_array, &shape_array_amount, &material_array,
                                       &material_array_size, file_path, sb__file_reader_function, NULL, TINYOBJ_FLAG_TRIANGULATE);
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
        su_LOG_ERRORF_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_MEDIUM, su_LOG_CONTEXT_MODEL_LOADING, "Could not load model: %s", error_reason);
    };

    *positions_count_out = attribute.num_vertices;
    if (!(*positions_count_out)) {
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_MEDIUM,
                       su_LOG_CONTEXT_MODEL_LOADING,
                       "Could not load position array");
        return false;
    }
    *position_array_out = su_CAST_M(su_Vec3*)(malloc(sizeof(su_Vec3) * (*positions_count_out)));
    if (!(*position_array_out)) {
        su_LOG_ERROR_M(su_LOG_TYPE_USER, su_LOG_ERROR_SEVERITY_MEDIUM,
                       su_LOG_CONTEXT_MODEL_LOADING, "Couldn't malloc positions");
        return false;
    }
    for (su_U64 i = 0; i < (*positions_count_out); ++i) {
        float pos_x = attribute.vertices[3 * i + 0];
        float pos_y = attribute.vertices[3 * i + 1];
        float pos_z = attribute.vertices[3 * i + 2];
        (*position_array_out)[i] = su_CAST_M(su_Vec3){pos_x, pos_y, pos_z};
    }

    *uv_count_out = attribute.num_texcoords;
    *uv_array_out = malloc(sizeof(su_Uv) * (*uv_count_out));
    for (su_U64 i = 0; i < (*uv_count_out); ++i) {
        float u = attribute.texcoords[2 * i + 0];
        float v = attribute.texcoords[2 * i + 1];
        (*uv_array_out)[i] = (su_Uv){u, v};
    }

    *index_count_out = attribute.num_faces;
    *index_array_out = malloc(sizeof(struct sb_VertexIndex) * (*index_count_out));
    for (su_U64 i = 0; i < (attribute.num_faces); ++i) {
        (*index_array_out)[i].vertex_index = su_CAST_M(su_U32)(attribute.faces[i].v_idx);
        (*index_array_out)[i].uv_index = su_CAST_M(su_U32) attribute.faces[i].vt_idx;
        (*index_array_out)[i].normal_index = su_CAST_M(su_U32) attribute.faces[i].vn_idx;
    }

    // These will not be changed to su_FREE, since they are already changed
    // through macro
    tinyobj_attrib_free(&attribute);
    tinyobj_shapes_free(shape_array, shape_array_amount);
    tinyobj_materials_free(material_array, material_array_size);
    free(sb_file_buffer_s);

    return true;
}

SA_INTERNAL void sb__file_reader_function(void* ctx, const char* filename, int is_mtl,
                                          const char* obj_filename2, char** buf, size_t* len) {
    su_NOT_USED_M(ctx); // suppress unused warning
    su_NOT_USED_M(is_mtl);
    su_NOT_USED_M(obj_filename2);

    FILE* file = fopen(filename, "rb");
    if (!file) {
        *buf = NULL;
        *len = 0;
        return;
    }

    fseek(file, 0, SEEK_END);
    su_U64 file_size = su_CAST_M(su_U64)(ftell(file));
    rewind(file);

    // Allocate or reallocate the static buffer if needed
    if (sb_file_buffer_len_s < file_size + 1) {
        free(sb_file_buffer_s); // free old buffer if any
        sb_file_buffer_s = (char*)malloc(file_size + 1);
        if (!sb_file_buffer_s) {
            fclose(file);
            *buf = NULL;
            *len = 0;
            return;
        }
        sb_file_buffer_len_s = file_size + 1;
    }

    fread(sb_file_buffer_s, 1, file_size, file);
    fclose(file);

    sb_file_buffer_s[file_size] = '\0'; // null-terminate just in case

    *buf = sb_file_buffer_s;
    *len = file_size;
}
