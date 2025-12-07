// TODO this whole file needs to be redone and modularized
#include "saci_platform/gfx/assets/model.h"

#include "saci_util/defines.h"
#include "saci_util/internal/general.h"
#include "saci_util/internal/log.h"
#include "saci_util/log.h"

// #define TINYOBJ_FREE
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c/tinyobj_loader_c.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* === Helper definitions === */

SACI_INTERNAL char* psaci_g_file_buffer = NULL; // buffer for the file reader function.
SACI_INTERNAL size_t psaci_g_file_buffer_len = 0;

SACI_INTERNAL SaciBool psaci__model_parse(const char* file_path,
                                          SaciVec3** position_array_out, SaciU64* positions_count_out,
                                          SaciUv** texcoord_array_out, SaciU64* texcoord_count_out,
                                          struct PSaciVertexIndex** index_array_out, SaciU64* index_count_out);

SACI_INTERNAL void psaci__file_reader_function(void* ctx, const char* filename, int is_mtl,
                                               const char* obj_filename, char** buf, size_t* len);

/* === Model Loading Implementation === */

#ifndef PSACI_MODEL_MESH_STRUCT
#  define PSACI_MODEL_MESH_STRUCT
struct PSaciVertexIndex {
    SaciU32 vertex_index;
    SaciU32 uv_index;
    SaciU32 normal_index;
};

struct PSaciModelMesh {
    SaciU64 index_count;
    SaciU64 uv_count;
    SaciU64 positions_count;
    struct PSaciVertexIndex* index_array;
    SaciUv* uv_array;
    SaciVec3* position_array;
};
#endif

struct PSaciModelMesh* psaci_model_mesh_load(const char* path)
{
    struct PSaciModelMesh* mesh = malloc(sizeof(struct PSaciModelMesh));
    SaciBool success = psaci__model_parse(path,
                                          &mesh->position_array,
                                          &mesh->positions_count,
                                          &mesh->uv_array,
                                          &mesh->uv_count,
                                          &mesh->index_array,
                                          &mesh->index_count);
    if (!success) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_MODEL_LOADING, "Couldn't load model");
        free(mesh);
        return NULL;
    }
    return mesh;
}

void psaci_model_delete(struct PSaciModelMesh* model_mesh)
{
    free(model_mesh->index_array);
    free(model_mesh->position_array);
    free(model_mesh->uv_array);

    model_mesh->index_array = NULL;
    model_mesh->position_array = NULL;
    model_mesh->uv_array = NULL;

    free(model_mesh);
    model_mesh = NULL;
}

void psaci_model_get_vertex_index_array(const struct PSaciModelMesh* model_mesh,
                                        struct PSaciVertexIndex** index_array_out,
                                        SaciU64* index_count_out)
{
    *index_array_out = malloc(sizeof(struct PSaciVertexIndex) *
                              model_mesh->index_count);
    if (!*index_array_out) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_MEDIUM,
                         SACI_LOG_CONTEXT_MODEL_LOADING,
                         "Could not allocate memory for index_array_out");
        return;
    }
    memcpy(*index_array_out, model_mesh->index_array,
           (sizeof(struct PSaciVertexIndex) * model_mesh->index_count));
    *index_count_out = model_mesh->index_count;
}

void psaci_model_get_separated_index_data(const struct PSaciModelMesh* model_mesh,
                                          SaciU32** vertex_index_out,
                                          SaciU32** uv_index_out,
                                          SaciU32** normal_index_out,
                                          SaciU64* index_array_count_out)
{
    *index_array_count_out = model_mesh->index_count;

    if (vertex_index_out) {
        *vertex_index_out = malloc(sizeof(SaciU32) * (*index_array_count_out));
    }
    if (uv_index_out) {
        *uv_index_out = malloc(sizeof(SaciU32) * (*index_array_count_out));
    }
    if (normal_index_out) {
        *normal_index_out = malloc(sizeof(SaciU32) * (*index_array_count_out));
    }

    for (SaciU64 i = 0; i < *index_array_count_out; ++i) {
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

void psaci_model_get_position_array(const struct PSaciModelMesh* model_mesh,
                                    SaciVec3** position_array_out,
                                    SaciU64* position_count_out)
{
    *position_array_out = malloc(sizeof(SaciVec3) * model_mesh->positions_count);
    *position_count_out = model_mesh->positions_count;
    memcpy(*position_array_out, model_mesh->position_array,
           (sizeof(SaciVec3) * model_mesh->positions_count));
}

void psaci_model_get_uv_array(const struct PSaciModelMesh* model_mesh,
                              SaciUv** uv_array_out,
                              SaciU64* uv_count_out)
{
    *uv_array_out = malloc(sizeof(SaciUv) * model_mesh->uv_count);
    *uv_count_out = model_mesh->uv_count;
    memcpy(*uv_array_out, model_mesh->uv_array,
           (sizeof(SaciUv) * model_mesh->uv_count));
}

void psaci_model_vertex_index_get_data(const struct PSaciVertexIndex* vertex_index,
                                       SaciU64 vertex_index_amount,
                                       SaciU32** vertex_index_out,
                                       SaciU32** uv_index_out,
                                       SaciU32** normal_index_out)
{
    *vertex_index_out = malloc(sizeof(SaciU32) * vertex_index_amount);
    *uv_index_out = malloc(sizeof(SaciU32) * vertex_index_amount);
    *normal_index_out = malloc(sizeof(SaciU32) * vertex_index_amount);
    for (SaciU64 i = 0; i < vertex_index_amount; ++i) {
        (*vertex_index_out)[i] = vertex_index[i].vertex_index;
        (*uv_index_out)[i] = vertex_index[i].uv_index;
        (*normal_index_out)[i] = vertex_index[i].normal_index;
    }
}

/* === Helper Implementation === */

SACI_INTERNAL SaciBool psaci__model_parse(const char* file_path,
                                          SaciVec3** position_array_out,
                                          SaciU64* positions_count_out,
                                          SaciUv** uv_array_out,
                                          SaciU64* uv_count_out,
                                          struct PSaciVertexIndex** index_array_out,
                                          SaciU64* index_count_out)
{
    tinyobj_attrib_t attribute = {0};
    tinyobj_shape_t* shape_array = NULL;
    SaciU64 shape_array_amount = 0;
    tinyobj_material_t* material_array = NULL;
    SaciU64 material_array_size = 0;

    SaciS32 success = tinyobj_parse_obj(&attribute, &shape_array, (size_t*)&shape_array_amount, &material_array,
                                        (size_t*)&material_array_size, file_path, psaci__file_reader_function, NULL, TINYOBJ_FLAG_TRIANGULATE);
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
        SACI_LOG_ERRORF_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_MEDIUM, SACI_LOG_CONTEXT_MODEL_LOADING, "Could not load model: %s", error_reason);
    };

    *positions_count_out = attribute.num_vertices;
    if (!(*positions_count_out)) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_MEDIUM,
                         SACI_LOG_CONTEXT_MODEL_LOADING,
                         "Could not load position array");
        return false;
    }
    *position_array_out = SACI_CAST_M(SaciVec3*)(malloc(sizeof(SaciVec3) * (*positions_count_out)));
    if (!(*position_array_out)) {
        SACI_LOG_ERROR_M(SACI_LOG_TYPE_USER, SACI_LOG_ERROR_SEVERITY_MEDIUM,
                         SACI_LOG_CONTEXT_MODEL_LOADING, "Couldn't malloc positions");
        return false;
    }
    for (SaciU64 i = 0; i < (*positions_count_out); ++i) {
        float pos_x = attribute.vertices[3 * i + 0];
        float pos_y = attribute.vertices[3 * i + 1];
        float pos_z = attribute.vertices[3 * i + 2];
        (*position_array_out)[i] = SACI_CAST_M(SaciVec3){pos_x, pos_y, pos_z};
    }

    *uv_count_out = attribute.num_texcoords;
    *uv_array_out = malloc(sizeof(SaciUv) * (*uv_count_out));
    for (SaciU64 i = 0; i < (*uv_count_out); ++i) {
        float u = attribute.texcoords[2 * i + 0];
        float v = attribute.texcoords[2 * i + 1];
        (*uv_array_out)[i] = (SaciUv){u, v};
    }

    *index_count_out = attribute.num_faces;
    *index_array_out = malloc(sizeof(struct PSaciVertexIndex) * (*index_count_out));
    for (SaciU64 i = 0; i < (attribute.num_faces); ++i) {
        (*index_array_out)[i].vertex_index = SACI_CAST_M(SaciU32)(attribute.faces[i].v_idx);
        (*index_array_out)[i].uv_index = SACI_CAST_M(SaciU32) attribute.faces[i].vt_idx;
        (*index_array_out)[i].normal_index = SACI_CAST_M(SaciU32) attribute.faces[i].vn_idx;
    }

    // These will not be changed to su_FREE, since they are already changed
    // through macro
    tinyobj_attrib_free(&attribute);
    tinyobj_shapes_free(shape_array, shape_array_amount);
    tinyobj_materials_free(material_array, material_array_size);
    free(psaci_g_file_buffer);

    return true;
}

SACI_INTERNAL void psaci__file_reader_function(void* ctx, const char* filename, int is_mtl,
                                               const char* obj_filename2, char** buf, size_t* len)
{
    SACI_NOT_USED_M(ctx); // suppress unused warning
    SACI_NOT_USED_M(is_mtl);
    SACI_NOT_USED_M(obj_filename2);

    FILE* file = fopen(filename, "rb");
    if (!file) {
        *buf = NULL;
        *len = 0;
        return;
    }

    fseek(file, 0, SEEK_END);
    SaciU64 file_size = SACI_CAST_M(SaciU64)(ftell(file));
    rewind(file);

    // Allocate or reallocate the static buffer if needed
    if (psaci_g_file_buffer_len < file_size + 1) {
        free(psaci_g_file_buffer); // free old buffer if any
        psaci_g_file_buffer = (char*)malloc(file_size + 1);
        if (!psaci_g_file_buffer) {
            fclose(file);
            *buf = NULL;
            *len = 0;
            return;
        }
        psaci_g_file_buffer_len = file_size + 1;
    }

    fread(psaci_g_file_buffer, 1, file_size, file);
    fclose(file);

    psaci_g_file_buffer[file_size] = '\0'; // null-terminate just in case

    *buf = psaci_g_file_buffer;
    *len = file_size;
}
