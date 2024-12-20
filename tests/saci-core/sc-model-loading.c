#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "saci-core/sc-gl.h"
#include "saci-testing.h"
#include "saci-utils/su-types.h"

static void __GenerateTriangulatedOBJFile(const char *path, sa_Vec3_t **positionArray,
                                          saci_u64 *positionArrayAmount, saci_Vec2 **texcoordArray,
                                          saci_u64 *texcoordArrayAmount,
                                          struct sc_VertexIndice_c **indicesArray,
                                          saci_u64 *indicesArrayAmount);

static void __GenerateNonTriangulatedOBJFile(const char *pathconst, sa_Vec3_t **positionArray,
                                             saci_u64 *positionArrayAmount,
                                             saci_Vec2 **texcoordArray,
                                             saci_u64 *texcoordArrayAmount,
                                             struct sc_VertexIndice_c **indicesArray,
                                             saci_u64 *indicesArrayAmount);

static void __FileReadingFunction(void *ctx, const char *filename, int isMtl,
                                  const char *objFilename, char **buf, size_t *len);
static void TestOBJParse(void);

void saci_TestModelLoading(void) { TestOBJParse(); }

static void TestOBJParse(void) {
    { /* Should load the information correctly for TRIANGULATED faces */
        const char *path = "./assets/test-file-1.obj";

        sa_Vec3_t *positionsExpected;
        saci_u64 positionsAmountExpected;
        saci_Vec2 *texcoordsExpected;
        saci_u64 texcoordAmountExpected;
        struct sc_VertexIndice *indicesExpected;
        saci_u64 indicesAmountExpected;

        __GenerateTriangulatedOBJFile(path, &positionsExpected, &positionsAmountExpected,
                                      &texcoordsExpected, &texcoordAmountExpected, &indicesExpected,
                                      &indicesAmountExpected);

        sa_Vec3_t *positions;
        saci_u64 positionsAmount;
        saci_Vec2 *texcoords;
        saci_u64 texcoordAmount;
        struct sc_VertexIndice *indices;
        saci_u64 indicesAmount;

        saci_Test_ClockBegin();
        // OBJ_Parse Function
        sc_OBJ_Parse(path, __FileReadingFunction, &positions, &positionsAmount, &texcoords,
                     &texcoordAmount, &indices, &indicesAmount);
        saci_Test_ClockEnd();
        SACI_TEST_ASSERT(saci_GetElapsedTimeMS() < 100, "Should load under 50 ms");

        SACI_TEST_ASSERT(positionsAmount == positionsAmountExpected,
                         "Should have correct vertex position amount");
        for (saci_u64 i = 0; i < positionsAmount; ++i) {
            SACI_TEST_ASSERT(SACI_TEST_VEC3_IS_EQUAL(positionsExpected[i], positions[i]),
                             "Should have correct vertex position values");
        }

        SACI_TEST_ASSERT(texcoordAmount == texcoordAmountExpected,
                         "Should have correct texcoord amount");
        for (saci_u64 i = 0; i < texcoordAmount; ++i) {
            SACI_TEST_ASSERT(SACI_TEST_VEC2_IS_EQUAL(texcoords[i], texcoordsExpected[i]),
                             "Should have correct texcoord values");
        }

        SACI_TEST_ASSERT(indicesAmount == indicesAmountExpected,
                         "Should have correct indice amount");
        for (saci_u64 i = 0; i < indicesAmount; ++i) {
            SACI_TEST_ASSERT(indices[i].vertexIndex == indicesExpected[i].vertexIndex,
                             "Should have vertex correct vertex indices");
            SACI_TEST_ASSERT(indices[i].texCoordIndex == indicesExpected[i].texCoordIndex,
                             "Should have vertex correct texcoord indices");
            /*
            // TODO: NOT YET IMPLEMENTED
            SACI_TEST_ASSERT(indices[i].normalIndex == indicesExpected[i].normalIndex,
                             "Should have vertex correct normal indices");
            */
        }
        if (positions) free(positions);
        if (texcoords) free(texcoords);
        if (indices) free(indices);
    }
    {
        const char *path = "./assets/test-file-2.obj";

        sa_Vec3_t *positionsExpected;
        saci_u64 positionsAmountExpected;
        saci_Vec2 *texcoordsExpected;
        saci_u64 texcoordAmountExpected;
        struct sc_VertexIndice *indicesExpected;
        saci_u64 indicesAmountExpected;

        __GenerateNonTriangulatedOBJFile(path, &positionsExpected, &positionsAmountExpected,
                                         &texcoordsExpected, &texcoordAmountExpected,
                                         &indicesExpected, &indicesAmountExpected);

        sa_Vec3_t *verticesPos;
        saci_u64 verticesPosAmount;
        saci_Vec2 *verticesTexCoords;
        saci_u64 verticesTexCoordsAmount;
        struct sc_VertexIndice *indices;
        saci_u64 indicesAmount;

        sc_OBJ_Parse(path, __FileReadingFunction, &verticesPos, &verticesPosAmount,
                     &verticesTexCoords, &verticesTexCoordsAmount, &indices, &indicesAmount);

        SACI_TEST_ASSERT(verticesPosAmount == positionsAmountExpected,
                         "Should have correct vertex position amount");
        for (saci_u64 i = 0; i < verticesPosAmount; ++i) {
            SACI_TEST_ASSERT(SACI_TEST_VEC3_IS_EQUAL(positionsExpected[i], verticesPos[i]),
                             "Should have correct vertex position");
        }
        SACI_TEST_ASSERT(verticesTexCoordsAmount == texcoordAmountExpected,
                         "Should have correct texcoord amount");
        for (saci_u64 i = 0; i < verticesTexCoordsAmount; ++i) {
            SACI_TEST_ASSERT(SACI_TEST_VEC2_IS_EQUAL(verticesTexCoords[i], texcoordsExpected[i]),
                             "Should have correct texcoord values");
        }
        SACI_TEST_ASSERT(indicesAmount == indicesAmountExpected,
                         "Should have correct indices amount");
        for (saci_u64 i = 0; i < indicesAmount; ++i) { // These are wrong
            SACI_TEST_ASSERT(indices[i].vertexIndex == indicesExpected[i].vertexIndex,
                             "Should have correct vertex indices");
            SACI_TEST_ASSERT(indices[i].texCoordIndex == indicesExpected[i].texCoordIndex,
                             "Should have correct texcoord indices");
            /**
            TODO
            NOT IMPLEMENTED
            SACI_TEST_ASSERT(indices[i].normalIndex == indicesExpected[i].normalIndex,
                             "Should have correct normal indices");
            */
        }
        free(verticesPos);
        free(verticesTexCoords);
        free(indices);
    }
}

static void __GenerateTriangulatedOBJFile(const char *pathconst, sa_Vec3_t **positionArray,
                                          saci_u64 *positionArrayAmount, saci_Vec2 **texcoordArray,
                                          saci_u64 *texcoordArrayAmount,
                                          struct sc_VertexIndice **indicesArray,
                                          saci_u64 *indicesArrayAmount) {
    srand(time(NULL));

    // Generate random amounts of vertices, texcoords, and indices
    saci_u64 verticesToBeGenerated = (rand() % 250 + 3);
    *positionArrayAmount = verticesToBeGenerated / 3; // ensures at least 3 vertices for a triangle
    *texcoordArrayAmount = *positionArrayAmount;
    *indicesArrayAmount = *positionArrayAmount * 3;

    // Allocate memory for vertices and texture coordinates
    *positionArray = (sa_Vec3_t *)malloc(sizeof(sa_Vec3_t) * (*positionArrayAmount));
    *texcoordArray = (saci_Vec2 *)malloc(sizeof(saci_Vec2) * (*texcoordArrayAmount));
    *indicesArray =
        (struct sc_VertexIndice *)malloc(sizeof(struct sc_VertexIndice) * (*indicesArrayAmount));

    // Fill position array with random values
    for (saci_u64 i = 0; i < (*positionArrayAmount); ++i) {
        float posX = (rand() % 100 / 10.0f) - 5.0f;
        float posY = (rand() % 100 / 10.0f) - 5.0f;
        float posZ = (rand() % 100 / 10.0f) - 5.0f;
        (*positionArray)[i] = (sa_Vec3_t){posX, posY, posZ};
    }

    // Fill texcoord array with random values between 0.0 and 1.0
    for (saci_u64 i = 0; i < (*texcoordArrayAmount); ++i) {
        float texU = (rand() % 1000) / 1000.0f;
        float texV = (rand() % 1000) / 1000.0f;
        (*texcoordArray)[i] = (saci_Vec2){texU, texV};
    }

    // Generate indices for triangles
    saci_u64 idx = 0;
    for (saci_u64 i = 0; i < (*indicesArrayAmount); i += 3) {
        (*indicesArray)[i + 0] = (struct sc_VertexIndice){idx, idx, idx};
        (*indicesArray)[i + 1] = (struct sc_VertexIndice){idx + 1, idx + 1, idx + 1};
        (*indicesArray)[i + 2] = (struct sc_VertexIndice){idx + 2, idx + 2, idx + 2};
        idx += 3;
    }

    // Write to file
    FILE *file = fopen(pathconst, "w");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    // Write vertices
    for (saci_u64 i = 0; i < *positionArrayAmount; ++i) {
        fprintf(file, "v %f %f %f\n", (*positionArray)[i].x, (*positionArray)[i].y,
                (*positionArray)[i].z);
    }

    // Write texture coordinates
    for (saci_u64 i = 0; i < *texcoordArrayAmount; ++i) {
        fprintf(file, "vt %f %f\n", (*texcoordArray)[i].x, (*texcoordArray)[i].y);
    }

    // Write faces (triangles)
    for (saci_u64 i = 0; i < *indicesArrayAmount; i += 3) {
        fprintf(file, "f %d/%d %d/%d %d/%d\n", (*indicesArray)[i].vertexIndex + 1,
                (*indicesArray)[i].texCoordIndex + 1, (*indicesArray)[i + 1].vertexIndex + 1,
                (*indicesArray)[i + 1].texCoordIndex + 1, (*indicesArray)[i + 2].vertexIndex + 1,
                (*indicesArray)[i + 2].texCoordIndex + 1);
    }

    fclose(file);
}

static void __GenerateNonTriangulatedOBJFile(const char *pathconst, sa_Vec3_t **positionArray,
                                             saci_u64 *positionArrayAmount,
                                             saci_Vec2 **texcoordArray,
                                             saci_u64 *texcoordArrayAmount,
                                             struct sc_VertexIndice **indicesArray,
                                             saci_u64 *indicesArrayAmount) {
    srand(time(NULL));

    // Generate random amounts of vertices and indices for quads
    saci_u64 verticesToBeGenerated = (rand() % 250 + 4);  // At least 4 vertices for a quad
    *positionArrayAmount = verticesToBeGenerated / 4 * 4; // Ensure divisible by 4
    *texcoordArrayAmount = *positionArrayAmount;
    *indicesArrayAmount = *positionArrayAmount; // Quad faces need 4 indices each

    // Allocate memory for vertices and texture coordinates
    *positionArray = (sa_Vec3_t *)malloc(sizeof(sa_Vec3_t) * (*positionArrayAmount));
    *texcoordArray = (saci_Vec2 *)malloc(sizeof(saci_Vec2) * (*texcoordArrayAmount));
    *indicesArray =
        (struct sc_VertexIndice *)malloc(sizeof(struct sc_VertexIndice) * (*indicesArrayAmount));

    // Fill position array with random values
    for (saci_u64 i = 0; i < (*positionArrayAmount); ++i) {
        float posX = (rand() % 100 / 10.0f) - 5.0f;
        float posY = (rand() % 100 / 10.0f) - 5.0f;
        float posZ = (rand() % 100 / 10.0f) - 5.0f;
        (*positionArray)[i] = (sa_Vec3_t){posX, posY, posZ};
    }

    // Fill texcoord array with random values between 0.0 and 1.0
    for (saci_u64 i = 0; i < (*texcoordArrayAmount); ++i) {
        float texU = (rand() % 1000) / 1000.0f;
        float texV = (rand() % 1000) / 1000.0f;
        (*texcoordArray)[i] = (saci_Vec2){texU, texV};
    }

    // Generate indices for quads
    saci_u64 idx = 0;
    for (saci_u64 i = 0; i < (*indicesArrayAmount); i += 4) {
        (*indicesArray)[i + 0] = (struct sc_VertexIndice){idx, idx, idx};
        (*indicesArray)[i + 1] = (struct sc_VertexIndice){idx + 1, idx + 1, idx + 1};
        (*indicesArray)[i + 2] = (struct sc_VertexIndice){idx + 2, idx + 2, idx + 2};
        (*indicesArray)[i + 3] = (struct sc_VertexIndice){idx + 3, idx + 3, idx + 3};
        idx += 4;
    }

    // Write to file
    FILE *file = fopen(pathconst, "w");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    // Write vertices
    for (saci_u64 i = 0; i < *positionArrayAmount; ++i) {
        fprintf(file, "v %f %f %f\n", (*positionArray)[i].x, (*positionArray)[i].y,
                (*positionArray)[i].z);
    }

    // Write texture coordinates
    for (saci_u64 i = 0; i < *texcoordArrayAmount; ++i) {
        fprintf(file, "vt %f %f\n", (*texcoordArray)[i].x, (*texcoordArray)[i].y);
    }

    // Write faces (quads)
    for (saci_u64 i = 0; i < *indicesArrayAmount; i += 4) {
        fprintf(file, "f %d/%d %d/%d %d/%d %d/%d\n", (*indicesArray)[i].vertexIndex + 1,
                (*indicesArray)[i].texCoordIndex + 1, (*indicesArray)[i + 1].vertexIndex + 1,
                (*indicesArray)[i + 1].texCoordIndex + 1, (*indicesArray)[i + 2].vertexIndex + 1,
                (*indicesArray)[i + 2].texCoordIndex + 1, (*indicesArray)[i + 3].vertexIndex + 1,
                (*indicesArray)[i + 3].texCoordIndex + 1);
    }

    fclose(file);
}

static void __FileReadingFunction(void *ctx, const char *filename, int isMtl,
                                  const char *objFilename, char **buf, size_t *len) {
    (void)ctx, (void)isMtl, (void)objFilename;
    // Open the file for reading
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        *buf = NULL;
        *len = 0;
        return;
    }

    // Seek to the end to get the file size
    fseek(file, 0, SEEK_END);
    *len = ftell(file);
    if (*len == 0) {
        *buf = NULL; // No need to allocate anything if the file is empty.
    }
    rewind(file);

    // Allocate memory for the buffer
    *buf = (char *)malloc(*len + 1); // +1 for null terminator
    if (!*buf) {
        fprintf(stderr, "Failed to allocate memory\n");
        fclose(file);
        *len = 0;
        return;
    }

    // Read file contents into buffer
    fread(*buf, 1, *len, file);
    (*buf)[*len] = '\0'; // Null terminate the buffer

    fclose(file);
}
