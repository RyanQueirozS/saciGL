#include <stdio.h>
#include <stdlib.h>
#include "saci-core/sc-model.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"
#include "saci-utils/su-debug.h"

// #define TINYOBJ_FREE sa_FREE // TODO
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c/tinyobj_loader_c.h"

/* === Helper definitions === */

static char** __sc_buffer = NULL; // buffer for the file reader function.

static void __sc_File_Reader_Function(void* ctx, const char* filename, int isMtl,
                                      const char* objFilename, char** buf, size_t* len);

/* === Model Loading Implementation === */

sa_bool_t sc_OBJ_Parse(const char* filePath,
                       sa_vec3_t** positions_out, sa_u64_t* positions_count_out,
                       sa_vec2_t** texcoords_out, sa_u64_t* texcoord_count_out,
                       struct sc_vertexIndice_c** indices_out, sa_u64_t* indices_count_out) {
    tinyobj_attrib_t attribute = {0};
    tinyobj_shape_t* shape_array = NULL;
    sa_u64_t shape_array_amount = 0;
    tinyobj_material_t* material_array = NULL;
    sa_u64_t material_array_size = 0;

    tinyobj_parse_obj(&attribute, &shape_array, &shape_array_amount, &material_array,
                      &material_array_size, filePath, __sc_File_Reader_Function, NULL, 0);

    *positions_count_out = attribute.num_vertices;
    if (!(*positions_count_out)) {
        fprintf(stderr, "Malloc Error\n");
    }
    *positions_out = sa_SCAST_TO_m(sa_vec3_t*) sa_MALLOC(sizeof(sa_vec3_t) * (*positions_count_out));
    if (!(*positions_out)) {
        sa_Log_Error_Print_m(sa_LOG_TYPE_ERROR, sa_LOG_SEVERITY_MEDIUM,
                             sa_LOG_CONTEXT_OBJ_LOADING, "Couldn't malloc positions");
        return false;
    }
    for (sa_u64_t i = 0; i < (*positions_count_out); ++i) {
        float pos_x = attribute.vertices[3 * i + 0];
        float pos_y = attribute.vertices[3 * i + 1];
        float pos_z = attribute.vertices[3 * i + 2];
        (*positions_out)[i] = sa_SCAST_TO_m(sa_vec3_t){pos_x, pos_y, pos_z};
    }

    *texcoord_count_out = attribute.num_texcoords;
    *texcoords_out = sa_SCAST_TO_m(sa_vec2_t*) sa_MALLOC(sizeof(sa_vec2_t) * (*texcoord_count_out));
    for (sa_u64_t i = 0; i < (*texcoord_count_out); ++i) {
        float u = attribute.texcoords[2 * i + 0];
        float v = attribute.texcoords[2 * i + 1];
        (*texcoords_out)[i] = (sa_vec2_t){u, v};
    }

    *indices_count_out = attribute.num_faces;
    *indices_out = sa_SCAST_TO_m(struct sc_vertexIndice_c*) sa_MALLOC(sizeof(struct sc_vertexIndice_c) * (*indices_count_out));
    for (sa_u64_t i = 0; i < (attribute.num_faces); ++i) {
        (*indices_out)[i].vertexIndex = sa_SCAST_TO_m(sa_u32_t)(attribute.faces[i].v_idx);
        (*indices_out)[i].texCoordIndex = sa_SCAST_TO_m(sa_u32_t) attribute.faces[i].vt_idx;
        (*indices_out)[i].normalIndex = sa_SCAST_TO_m(sa_u32_t) attribute.faces[i].vn_idx;
    }

    // These will not be changed to sa_FREE, since they are already changed
    // through macro
    tinyobj_attrib_free(&attribute);
    tinyobj_shapes_free(shape_array, shape_array_amount);
    tinyobj_materials_free(material_array, material_array_size);
    sa_FREE(__sc_buffer);

    return true;
}

/* === Helper Implementation === */

static void __sc_File_Reader_Function(void* ctx, const char* filename, int isMtl,
                                      const char* objFilename2, char** buf, size_t* len) {
    sa_NOT_USED(buf);
    sa_NOT_USED(ctx);
    sa_NOT_USED(isMtl);
    sa_NOT_USED(objFilename2);
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Unable to open file '%s'\n", filename);
        *__sc_buffer = NULL;
        *len = 0;
        return;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Error: Failed to determine file size for '%s'\n", filename);
        fclose(file);
        *__sc_buffer = NULL;
        *len = 0;
        return;
    }

    sa_s64_t file_size = ftell(file);
    if (file_size < 0) {
        fprintf(stderr, "Error: Failed to get file size for '%s'\n", filename);
        fclose(file);
        *__sc_buffer = NULL;
        *len = 0;
        return;
    }

    *__sc_buffer = (char*)malloc(sa_SCAST_TO_m(sa_u64_t)(file_size));
    if (!*__sc_buffer) {
        fprintf(stderr, "Error: Memory allocation failed for file '%s'\n", filename);
        fclose(file);
        *len = 0;
        return;
    }

    rewind(file);
    size_t bytes_read = fread(*__sc_buffer, 1, sa_SCAST_TO_m(sa_u64_t)(file_size), file);
    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "Error: File read error for '%s'\n", filename);
        sa_FREE(*__sc_buffer);
        *__sc_buffer = NULL;
        *len = 0;
        fclose(file);
        return;
    }

    *len = (size_t)file_size;
    fclose(file);
}
