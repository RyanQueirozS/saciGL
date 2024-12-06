#include <stdio.h>
#include <stdlib.h>
#include "saci-core/sc-gl.h"
#include "saci-utils/su-types.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj-loader_c/tinyobj_loader_c.h"

sa_Bool_t sc_OBJ_Parse(const char* filePath, sc_OBJ_ModelFileReadingFunction fileReader,
                       sa_Vec3_t** positions, sa_U64_t* positionsCount, sa_Vec2_t** texcoords,
                       sa_U64_t* texcoordCount, struct sc_VertexIndice** indices,
                       sa_U64_t* indicesCount) {
    tinyobj_attrib_t attribute = {0};
    tinyobj_shape_t* shapeArray = NULL;
    sa_U64_t shapeArrayAmount = 0;
    tinyobj_material_t* materialArray = NULL;
    sa_U64_t materialArraySize = 0;

    tinyobj_parse_obj(&attribute, &shapeArray, &shapeArrayAmount, &materialArray,
                      &materialArraySize, filePath, fileReader, NULL, 0);

    *positionsCount = attribute.num_vertices;
    if (!(*positionsCount)) {
        fprintf(stderr, "Malloc Error\n");
    }
    *positions = (sa_Vec3_t*)malloc(sizeof(sa_Vec3_t) * (*positionsCount));
    if (!(*positions)) {
        fprintf(stderr, "Malloc Error\n");
    }
    for (sa_U64_t i = 0; i < (*positionsCount); ++i) {
        float posX = attribute.vertices[3 * i + 0];
        float posY = attribute.vertices[3 * i + 1];
        float posZ = attribute.vertices[3 * i + 2];
        (*positions)[i] = (sa_Vec3_t){posX, posY, posZ};
    }

    *texcoordCount = attribute.num_texcoords;
    *texcoords = (sa_Vec2_t*)malloc(sizeof(sa_Vec2_t) * (*texcoordCount));
    for (sa_U64_t i = 0; i < (*texcoordCount); ++i) {
        float u = attribute.texcoords[2 * i + 0];
        float v = attribute.texcoords[2 * i + 1];
        (*texcoords)[i] = (sa_Vec2_t){u, v};
    }

    *indicesCount = attribute.num_faces;
    *indices = (struct sc_VertexIndice*)malloc(sizeof(struct sc_VertexIndice) * (*indicesCount));
    for (sa_U64_t i = 0; i < (attribute.num_faces); ++i) {
        (*indices)[i].vertexIndex = attribute.faces[i].v_idx;
        (*indices)[i].texCoordIndex = attribute.faces[i].vt_idx;
        (*indices)[i].normalIndex = attribute.faces[i].vn_idx;
    }

    tinyobj_attrib_free(&attribute);
    tinyobj_shapes_free(shapeArray, shapeArrayAmount);
    tinyobj_materials_free(materialArray, materialArraySize);

    return true;
}
