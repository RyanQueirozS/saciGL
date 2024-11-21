#include <stdio.h>
#include <stdlib.h>
#include "saci-core/sc-gl.h"
#include "saci-utils/su-types.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj-loader_c/tinyobj_loader_c.h"

saci_Bool sc_OBJ_Parse(const char* filePath, sc_OBJ_ModelFileReadingFunction fileReader,
                       saci_Vec3** positions, saci_u64* positionsCount, saci_Vec2** texcoords,
                       saci_u64* texcoordCount, struct sc_VertexIndice** indices,
                       saci_u64* indicesCount) {
    tinyobj_attrib_t attribute = {0};
    tinyobj_shape_t* shapeArray = NULL;
    saci_u64 shapeArrayAmount = 0;
    tinyobj_material_t* materialArray = NULL;
    saci_u64 materialArraySize = 0;

    tinyobj_parse_obj(&attribute, &shapeArray, &shapeArrayAmount, &materialArray,
                      &materialArraySize, filePath, fileReader, NULL, 0);

    *positionsCount = attribute.num_vertices;
    if (!(*positionsCount)) {
        fprintf(stderr, "Malloc Error\n");
    }
    *positions = (saci_Vec3*)malloc(sizeof(saci_Vec3) * (*positionsCount));
    if (!(*positions)) {
        fprintf(stderr, "Malloc Error\n");
    }
    for (saci_u64 i = 0; i < (*positionsCount); ++i) {
        float posX = attribute.vertices[3 * i + 0];
        float posY = attribute.vertices[3 * i + 1];
        float posZ = attribute.vertices[3 * i + 2];
        (*positions)[i] = (saci_Vec3){posX, posY, posZ};
    }

    *texcoordCount = attribute.num_texcoords;
    *texcoords = (saci_Vec2*)malloc(sizeof(saci_Vec2) * (*texcoordCount));
    for (saci_u64 i = 0; i < (*texcoordCount); ++i) {
        float u = attribute.texcoords[2 * i + 0];
        float v = attribute.texcoords[2 * i + 1];
        (*texcoords)[i] = (saci_Vec2){u, v};
    }

    *indicesCount = attribute.num_faces;
    *indices = (struct sc_VertexIndice*)malloc(sizeof(struct sc_VertexIndice) * (*indicesCount));
    for (saci_u64 i = 0; i < (attribute.num_faces); ++i) {
        (*indices)[i].vertexIndex = attribute.faces[i].v_idx;
        (*indices)[i].texCoordIndex = attribute.faces[i].vt_idx;
        (*indices)[i].normalIndex = attribute.faces[i].vn_idx;
    }

    tinyobj_attrib_free(&attribute);
    tinyobj_shapes_free(shapeArray, shapeArrayAmount);
    tinyobj_materials_free(materialArray, materialArraySize);

    return true;
}
