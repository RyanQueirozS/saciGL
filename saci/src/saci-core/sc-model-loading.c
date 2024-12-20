#include <stdio.h>
#include <stdlib.h>
#include "saci-core/sc-gl.h"
#include "saci-utils/su-types.h"
#include "saci-utils/su-general.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj-loader_c/tinyobj_loader_c.h"

sa_bool_t sc_OBJ_Parse(const char* filePath, sc_OBJ_File_Reading_Function file_reader,
                       sa_Vec3_t** positions_out, sa_u64_t* positions_count_out,
                       sa_Vec2_t** texcoords_out, sa_u64_t* texcoord_count_out,
                       struct sc_VertexIndice_c** indices_out, sa_u64_t* indices_count_out) {
    tinyobj_attrib_t attribute = {0};
    tinyobj_shape_t* shape_array = NULL;
    sa_u64_t shape_array_amount = 0;
    tinyobj_material_t* material_array = NULL;
    sa_u64_t material_array_size = 0;

    tinyobj_parse_obj(&attribute, &shape_array, &shape_array_amount, &material_array,
                      &material_array_size, filePath, file_reader, NULL, 0);

    *positions_count_out = attribute.num_vertices;
    if (!(*positions_count_out)) {
        fprintf(stderr, "Malloc Error\n");
    }
    *positions_out = sa_SCAST_TO_m(sa_Vec3_t*) sa_MALLOC(sizeof(sa_Vec3_t) * (*positions_count_out));
    if (!(*positions_out)) {
        // TODO error handling
        fprintf(stderr, "Malloc Error\n");
    }
    for (sa_u64_t i = 0; i < (*positions_count_out); ++i) {
        float pos_x = attribute.vertices[3 * i + 0];
        float pos_y = attribute.vertices[3 * i + 1];
        float pos_z = attribute.vertices[3 * i + 2];
        (*positions_out)[i] = sa_SCAST_TO_m(sa_Vec3_t){pos_x, pos_y, pos_z};
    }

    *texcoord_count_out = attribute.num_texcoords;
    *texcoords_out = sa_SCAST_TO_m(sa_Vec2_t*) sa_MALLOC(sizeof(sa_Vec2_t) * (*texcoord_count_out));
    for (sa_u64_t i = 0; i < (*texcoord_count_out); ++i) {
        float u = attribute.texcoords[2 * i + 0];
        float v = attribute.texcoords[2 * i + 1];
        (*texcoords_out)[i] = (sa_Vec2_t){u, v};
    }

    *indices_count_out = attribute.num_faces;
    *indices_out = sa_SCAST_TO_m(struct sc_VertexIndice_c*) sa_MALLOC(sizeof(struct sc_VertexIndice_c) * (*indices_count_out));
    for (sa_u64_t i = 0; i < (attribute.num_faces); ++i) {
        (*indices_out)[i].vertexIndex = attribute.faces[i].v_idx;
        (*indices_out)[i].texCoordIndex = attribute.faces[i].vt_idx;
        (*indices_out)[i].normalIndex = attribute.faces[i].vn_idx;
    }

    tinyobj_attrib_free(&attribute);
    tinyobj_shapes_free(shape_array, shape_array_amount);
    tinyobj_materials_free(material_array, material_array_size);

    return true;
}
