#include <stdio.h>
#include <stdlib.h>
#include "saci-core/sc-gl.h"
#include "saci-utils/su-types.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj-loader_c/tinyobj_loader_c.h"

sa_Bool_t sc_OBJ_Parse(const char* filePath, sc_OBJ_File_Reading_Function file_reader,
                       sa_Vec3_t** positions_out, sa_U64_t* positions_count_out,
                       sa_Vec2_t** texcoords_out, sa_U64_t* texcoord_count_out,
                       struct sc_Vertex_Indice** indices_out, sa_U64_t* indices_count_out) {
    tinyobj_attrib_t attribute = {0};
    tinyobj_shape_t* shapeArray = NULL;
    sa_U64_t shapeArrayAmount = 0;
    tinyobj_material_t* materialArray = NULL;
    sa_U64_t materialArraySize = 0;

    tinyobj_parse_obj(&attribute, &shapeArray, &shapeArrayAmount, &materialArray,
                      &materialArraySize, filePath, file_reader, NULL, 0);

    *positions_count_out = attribute.num_vertices;
    if (!(*positions_count_out)) {
        fprintf(stderr, "Malloc Error\n");
    }
    *positions_out = (sa_Vec3_t*)malloc(sizeof(sa_Vec3_t) * (*positions_count_out));
    if (!(*positions_out)) {
        fprintf(stderr, "Malloc Error\n");
    }
    for (sa_U64_t i = 0; i < (*positions_count_out); ++i) {
        float posX = attribute.vertices[3 * i + 0];
        float posY = attribute.vertices[3 * i + 1];
        float posZ = attribute.vertices[3 * i + 2];
        (*positions_out)[i] = (sa_Vec3_t){posX, posY, posZ};
    }

    *texcoord_count_out = attribute.num_texcoords;
    *texcoords_out = (sa_Vec2_t*)malloc(sizeof(sa_Vec2_t) * (*texcoord_count_out));
    for (sa_U64_t i = 0; i < (*texcoord_count_out); ++i) {
        float u = attribute.texcoords[2 * i + 0];
        float v = attribute.texcoords[2 * i + 1];
        (*texcoords_out)[i] = (sa_Vec2_t){u, v};
    }

    *indices_count_out = attribute.num_faces;
    *indices_out = (struct sc_Vertex_Indice*)malloc(sizeof(struct sc_Vertex_Indice) * (*indices_count_out));
    for (sa_U64_t i = 0; i < (attribute.num_faces); ++i) {
        (*indices_out)[i].vertexIndex = attribute.faces[i].v_idx;
        (*indices_out)[i].texCoordIndex = attribute.faces[i].vt_idx;
        (*indices_out)[i].normalIndex = attribute.faces[i].vn_idx;
    }

    tinyobj_attrib_free(&attribute);
    tinyobj_shapes_free(shapeArray, shapeArrayAmount);
    tinyobj_materials_free(materialArray, materialArraySize);

    return true;
}
