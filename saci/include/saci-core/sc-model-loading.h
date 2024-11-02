#ifndef __SACI_CORE_SC_MODEL_LOADING_H__
#define __SACI_CORE_SC_MODEL_LOADING_H__

#include "saci-utils/su-types.h"

typedef void (*sc_fileReadingFunction)(const char* path, char** buffer, saci_u64* length);

struct sc_VertexIndice {
    int vertexIndex;
    int texCoordIndex;
    int normalIndex;
};

saci_Vec3* sc_OBJ_ModelIndiceToVec3(const struct sc_VertexIndice* indices);

saci_Bool sc_OBJ_Load(const char* path, sc_fileReadingFunction fileReader, saci_Vec3** verticesPos,
                      saci_u64* verticesAmount, saci_Vec2** verticesTexCoords,
                      saci_u64* verticesTexCoordsAmount, struct sc_VertexIndice** indices,
                      saci_u64* indicesAmount);

#endif
