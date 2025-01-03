#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include "saci-core/sc-gl.h"
#include "saci-testing.h"
#include "saci-utils/su-types.h"

/* === Helper functions === */

static void __OBJ_Gen_Triangulated_File(const char *path, sa_vec3_t **positionArray,
                                        sa_u64_t *positionArrayAmount, sa_vec2_t **texcoordArray,
                                        sa_u64_t *texcoordArrayAmount,
                                        struct sc_vertexIndice_c **indicesArray,
                                        sa_u64_t *indicesArrayAmount);

static void __OBJ_Gen_Nontriangulated_File(const char *pathconst, sa_vec3_t **positionArray,
                                           sa_u64_t *positionArrayAmount,
                                           sa_vec2_t **texcoordArray,
                                           sa_u64_t *texcoordArrayAmount,
                                           struct sc_vertexIndice_c **indicesArray,
                                           sa_u64_t *indicesArrayAmount);

/* === Test Functions === */

static void __Test_OBJ_Parse(void);

/* === Main Test Function === */

void Test_Unit_Model_Loading(void) { __Test_OBJ_Parse(); }

/* === Test Functions implementation === */

static void __Test_OBJ_Parse(void) {
    { /* Should load the information correctly for TRIANGULATED faces */
        const char *path = "./assets/test-file-1.obj";

        sa_vec3_t *positionsExpected;
        sa_u64_t positionsAmountExpected;
        sa_vec2_t *texcoordsExpected;
        sa_u64_t texcoordAmountExpected;
        struct sc_vertexIndice_c *indicesExpected;
        sa_u64_t indicesAmountExpected;

        __OBJ_Gen_Triangulated_File(path, &positionsExpected, &positionsAmountExpected,
                                    &texcoordsExpected, &texcoordAmountExpected, &indicesExpected,
                                    &indicesAmountExpected);

        sa_vec3_t *positions;
        sa_u64_t positionsAmount;
        sa_vec2_t *texcoords;
        sa_u64_t texcoordAmount;
        struct sc_vertexIndice_c *indices;
        sa_u64_t indicesAmount;

        saci_Test_ClockBegin();
        SACI_TEST_ASSERT(sc_OBJ_Parse(path, &positions, &positionsAmount, &texcoords,
                                      &texcoordAmount, &indices, &indicesAmount),
                         "should return true (success)");
        saci_Test_ClockEnd();
        SACI_TEST_ASSERT(saci_Test_GetElapsedTimeMS() < 50, "Should load under 50 ms"); // This should depend on hardware. But LOL

        SACI_TEST_ASSERT(positionsAmount == positionsAmountExpected,
                         "Should have correct vertex position amount");
        for (sa_u64_t i = 0; i < positionsAmount; ++i) {
            SACI_TEST_ASSERT(SACI_TEST_VEC3_IS_EQUAL(positionsExpected[i], positions[i]),
                             "Should have correct vertex position values");
        }

        SACI_TEST_ASSERT(texcoordAmount == texcoordAmountExpected,
                         "Should have correct texcoord amount");
        for (sa_u64_t i = 0; i < texcoordAmount; ++i) {
            SACI_TEST_ASSERT(SACI_TEST_VEC2_IS_EQUAL(texcoords[i], texcoordsExpected[i]),
                             "Should have correct texcoord values");
        }

        SACI_TEST_ASSERT(indicesAmount == indicesAmountExpected,
                         "Should have correct indice amount");
        for (sa_u64_t i = 0; i < indicesAmount; ++i) {
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
    { /* Should load the information correctly for NON-TRIANGULATED faces */
        const char *path = "./assets/test-file-2.obj";

        sa_vec3_t *positionsExpected;
        sa_u64_t positionsAmountExpected;
        sa_vec2_t *texcoordsExpected;
        sa_u64_t texcoordAmountExpected;
        struct sc_vertexIndice_c *indicesExpected;
        sa_u64_t indicesAmountExpected;

        __OBJ_Gen_Nontriangulated_File(path, &positionsExpected, &positionsAmountExpected,
                                       &texcoordsExpected, &texcoordAmountExpected,
                                       &indicesExpected, &indicesAmountExpected);

        sa_vec3_t *vertice_pos_array;
        sa_u64_t vertice_pos_array_amount;
        sa_vec2_t *vertice_texcoord_array;
        sa_u64_t vertice_texcoord_array_amount;
        struct sc_vertexIndice_c *indice_array;
        sa_u64_t indice_array_amount;

        sc_OBJ_Parse(path, &vertice_pos_array, &vertice_pos_array_amount,
                     &vertice_texcoord_array, &vertice_texcoord_array_amount, &indice_array, &indice_array_amount);

        SACI_TEST_ASSERT(vertice_pos_array_amount == positionsAmountExpected,
                         "Should have correct vertex position amount");
        for (sa_u64_t i = 0; i < vertice_pos_array_amount; ++i) {
            SACI_TEST_ASSERT(SACI_TEST_VEC3_IS_EQUAL(positionsExpected[i], vertice_pos_array[i]),
                             "Should have correct vertex position");
        }
        SACI_TEST_ASSERT(vertice_texcoord_array_amount == texcoordAmountExpected,
                         "Should have correct texcoord amount");
        for (sa_u64_t i = 0; i < vertice_texcoord_array_amount; ++i) {
            SACI_TEST_ASSERT(SACI_TEST_VEC2_IS_EQUAL(vertice_texcoord_array[i], texcoordsExpected[i]),
                             "Should have correct texcoord values");
        }
        SACI_TEST_ASSERT(indice_array_amount == indicesAmountExpected,
                         "Should have correct indices amount");
        for (sa_u64_t i = 0; i < indice_array_amount; ++i) { // These are wrong
            SACI_TEST_ASSERT(indice_array[i].vertexIndex == indicesExpected[i].vertexIndex,
                             "Should have correct vertex indices");
            SACI_TEST_ASSERT(indice_array[i].texCoordIndex == indicesExpected[i].texCoordIndex,
                             "Should have correct texcoord indices");
            /*
            TODO
            NOT IMPLEMENTED
            SACI_TEST_ASSERT(indices[i].normalIndex == indicesExpected[i].normalIndex,
                             "Should have correct normal indices");
            */
        }
        if (vertice_pos_array) free(vertice_pos_array);
        if (vertice_texcoord_array) free(vertice_texcoord_array);
        if (indice_array) free(indice_array);
    }
}

/* === Helper function implementation === */

static void __OBJ_Gen_Triangulated_File(const char *path, sa_vec3_t **positionArray,
                                        sa_u64_t *positionArrayAmount, sa_vec2_t **texcoordArray,
                                        sa_u64_t *texcoordArrayAmount,
                                        struct sc_vertexIndice_c **indicesArray,
                                        sa_u64_t *indicesArrayAmount) {
    sa_u64_t verticesToBeGenerated = (rand() % 250 + 3); // +3 to ensure there is at least 3 vertices
    *positionArrayAmount = verticesToBeGenerated / 3;
    *texcoordArrayAmount = *positionArrayAmount;
    *indicesArrayAmount = *positionArrayAmount * 3;

    // Allocate memory for vertices and texture coordinates
    *positionArray = (sa_vec3_t *)malloc(sizeof(sa_vec3_t) * (*positionArrayAmount));
    *texcoordArray = (sa_vec2_t *)malloc(sizeof(sa_vec2_t) * (*texcoordArrayAmount));
    *indicesArray =
        (struct sc_vertexIndice_c *)malloc(sizeof(struct sc_vertexIndice_c) * (*indicesArrayAmount));

    // Fill position array with random values
    for (sa_u64_t i = 0; i < (*positionArrayAmount); ++i) {
        float posX = (rand() % 100 / 10.0f) - 5.0f;
        float posY = (rand() % 100 / 10.0f) - 5.0f;
        float posZ = (rand() % 100 / 10.0f) - 5.0f;
        (*positionArray)[i] = (sa_vec3_t){posX, posY, posZ};
    }

    // Fill texcoord array with random values between 0.0 and 1.0
    for (sa_u64_t i = 0; i < (*texcoordArrayAmount); ++i) {
        float texU = (rand() % 1000) / 1000.0f;
        float texV = (rand() % 1000) / 1000.0f;
        (*texcoordArray)[i] = (sa_vec2_t){texU, texV};
    }

    // Generate indices for triangles
    sa_u64_t idx = 0;
    for (sa_u64_t i = 0; i < (*indicesArrayAmount); i += 3) {
        (*indicesArray)[i + 0] = (struct sc_vertexIndice_c){idx, idx, idx};
        (*indicesArray)[i + 1] = (struct sc_vertexIndice_c){idx + 1, idx + 1, idx + 1};
        (*indicesArray)[i + 2] = (struct sc_vertexIndice_c){idx + 2, idx + 2, idx + 2};
        idx += 3;
    }

    // Write to file
    FILE *file = fopen(path, "w");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    // Write vertices
    for (sa_u64_t i = 0; i < *positionArrayAmount; ++i) {
        fprintf(file, "v %f %f %f\n", (*positionArray)[i].m_x, (*positionArray)[i].m_y,
                (*positionArray)[i].m_z);
    }

    // Write texture coordinates
    for (sa_u64_t i = 0; i < *texcoordArrayAmount; ++i) {
        fprintf(file, "vt %f %f\n", (*texcoordArray)[i].m_x, (*texcoordArray)[i].m_y);
    }

    // Write faces (triangles)
    for (sa_u64_t i = 0; i < *indicesArrayAmount; i += 3) {
        fprintf(file, "f %d/%d %d/%d %d/%d\n", (*indicesArray)[i].vertexIndex + 1,
                (*indicesArray)[i].texCoordIndex + 1, (*indicesArray)[i + 1].vertexIndex + 1,
                (*indicesArray)[i + 1].texCoordIndex + 1, (*indicesArray)[i + 2].vertexIndex + 1,
                (*indicesArray)[i + 2].texCoordIndex + 1);
    }

    fclose(file);
}

static void __OBJ_Gen_Nontriangulated_File(const char *pathconst, sa_vec3_t **positionArray,
                                           sa_u64_t *positionArrayAmount,
                                           sa_vec2_t **texcoordArray,
                                           sa_u64_t *texcoordArrayAmount,
                                           struct sc_vertexIndice_c **indicesArray,
                                           sa_u64_t *indicesArrayAmount) {
    // Generate random amounts of vertices and indices for quads
    sa_u64_t verticesToBeGenerated = (rand() % 250 + 4);  // At least 4 vertices for a quad
    *positionArrayAmount = verticesToBeGenerated / 4 * 4; // Ensure divisible by 4
    *texcoordArrayAmount = *positionArrayAmount;
    *indicesArrayAmount = *positionArrayAmount; // Quad faces need 4 indices each

    // Allocate memory for vertices and texture coordinates
    *positionArray = (sa_vec3_t *)malloc(sizeof(sa_vec3_t) * (*positionArrayAmount));
    *texcoordArray = (sa_vec2_t *)malloc(sizeof(sa_vec2_t) * (*texcoordArrayAmount));
    *indicesArray =
        (struct sc_vertexIndice_c *)malloc(sizeof(struct sc_vertexIndice_c) * (*indicesArrayAmount));

    // Fill position array with random values
    for (sa_u64_t i = 0; i < (*positionArrayAmount); ++i) {
        float posX = (rand() % 100 / 10.0f) - 5.0f;
        float posY = (rand() % 100 / 10.0f) - 5.0f;
        float posZ = (rand() % 100 / 10.0f) - 5.0f;
        (*positionArray)[i] = (sa_vec3_t){posX, posY, posZ};
    }

    // Fill texcoord array with random values between 0.0 and 1.0
    for (sa_u64_t i = 0; i < (*texcoordArrayAmount); ++i) {
        float texU = (rand() % 1000) / 1000.0f;
        float texV = (rand() % 1000) / 1000.0f;
        (*texcoordArray)[i] = (sa_vec2_t){texU, texV};
    }

    // Generate indices for quads
    sa_u64_t idx = 0;
    for (sa_u64_t i = 0; i < (*indicesArrayAmount); i += 4) {
        (*indicesArray)[i + 0] = (struct sc_vertexIndice_c){idx, idx, idx};
        (*indicesArray)[i + 1] = (struct sc_vertexIndice_c){idx + 1, idx + 1, idx + 1};
        (*indicesArray)[i + 2] = (struct sc_vertexIndice_c){idx + 2, idx + 2, idx + 2};
        (*indicesArray)[i + 3] = (struct sc_vertexIndice_c){idx + 3, idx + 3, idx + 3};
        idx += 4;
    }

    // Write to file
    FILE *file = fopen(pathconst, "w");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    // Write vertices
    for (sa_u64_t i = 0; i < *positionArrayAmount; ++i) {
        fprintf(file, "v %f %f %f\n", (*positionArray)[i].m_x, (*positionArray)[i].m_y,
                (*positionArray)[i].m_z);
    }

    // Write texture coordinates
    for (sa_u64_t i = 0; i < *texcoordArrayAmount; ++i) {
        fprintf(file, "vt %f %f\n", (*texcoordArray)[i].m_x, (*texcoordArray)[i].m_y);
    }

    // Write faces (quads)
    for (sa_u64_t i = 0; i < *indicesArrayAmount; i += 4) {
        fprintf(file, "f %d/%d %d/%d %d/%d %d/%d\n", (*indicesArray)[i].vertexIndex + 1,
                (*indicesArray)[i].texCoordIndex + 1, (*indicesArray)[i + 1].vertexIndex + 1,
                (*indicesArray)[i + 1].texCoordIndex + 1, (*indicesArray)[i + 2].vertexIndex + 1,
                (*indicesArray)[i + 2].texCoordIndex + 1, (*indicesArray)[i + 3].vertexIndex + 1,
                (*indicesArray)[i + 3].texCoordIndex + 1);
    }

    fclose(file);
}
