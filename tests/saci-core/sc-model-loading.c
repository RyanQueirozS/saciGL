#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "saci-core/sc-gl.h"
#include "saci-testing.h"

/**
 * @brief
 *
 * @param buffer
 * @param length
 * @param verticesPos
 * @param verticesAmount
 * @param verticesTexCoords
 * @param verticesTexCoordsAmount
 * @param normals
 * @param normalsAmount
 * @param sc_VertexIndice
 * @param indices
 * @param indicesAmount
 */
static void __generateRandomBuffer(char** buffer, saci_u64* length, saci_Vec3** verticesPos,
                                   saci_u64* verticesAmount, saci_Vec2** verticesTexCoords,
                                   saci_u64* verticesTexCoordsAmount, saci_Vec3** normals,
                                   saci_u64* normalsAmount, struct sc_VertexIndice** indices,
                                   saci_u64* indicesAmount) { // Seed the random number generator
    srand((unsigned int)time(NULL));

    // Generate random vertices
    *verticesAmount = 10 + rand() % 10000; // random between 10 and 20 vertices
    *verticesPos = (saci_Vec3*)malloc(*verticesAmount * sizeof(saci_Vec3));
    for (saci_u64 i = 0; i < *verticesAmount; ++i) {
        (*verticesPos)[i].x = (float)(rand() % 100) / 10.0f;
        (*verticesPos)[i].y = (float)(rand() % 100) / 10.0f;
        (*verticesPos)[i].z = (float)(rand() % 100) / 10.0f;
    }

    // Generate random texture coordinates
    *verticesTexCoordsAmount = *verticesAmount; // same amount as vertices
    *verticesTexCoords = (saci_Vec2*)malloc(*verticesTexCoordsAmount * sizeof(saci_Vec2));
    for (saci_u64 i = 0; i < *verticesTexCoordsAmount; ++i) {
        (*verticesTexCoords)[i].x = (float)(rand() % 100) / 100.0f;
        (*verticesTexCoords)[i].y = (float)(rand() % 100) / 100.0f;
    }

    // Generate random normals
    *normalsAmount = *verticesAmount; // same amount as vertices
    *normals = (saci_Vec3*)malloc(*normalsAmount * sizeof(saci_Vec3));
    for (saci_u64 i = 0; i < *normalsAmount; ++i) {
        (*normals)[i].x = (float)(rand() % 100) / 10.0f;
        (*normals)[i].y = (float)(rand() % 100) / 10.0f;
        (*normals)[i].z = (float)(rand() % 100) / 10.0f;
    }

    // Generate indices for faces (triangular faces)
    *indicesAmount = (*verticesAmount >= 3) ? *verticesAmount - 2 : 0;
    *indices = (struct sc_VertexIndice*)malloc(*indicesAmount * sizeof(struct sc_VertexIndice) *
                                               3); // 3 vertices per face
    for (saci_u64 i = 0; i < *indicesAmount; ++i) {
        (*indices)[i].vertexIndex = (rand() % *verticesAmount) + 1; // OBJ is 1-indexed
        (*indices)[i].texCoordIndex = (rand() % *verticesTexCoordsAmount) + 1;
        (*indices)[i].normalIndex = (rand() % *normalsAmount) + 1;
    }

    // Calculate buffer size and prepare for OBJ format
    *length = 0;
    saci_u64 estimated_size =
        256 * (*verticesAmount + *verticesTexCoordsAmount + *normalsAmount + *indicesAmount);
    *buffer = (char*)malloc(estimated_size);
    char* buf_ptr = *buffer;

    // Write vertices (v) to buffer
    for (saci_u64 i = 0; i < *verticesAmount; ++i) {
        int written = snprintf(buf_ptr, estimated_size - *length, "v %.2f %.2f %.2f\n",
                               (*verticesPos)[i].x, (*verticesPos)[i].y, (*verticesPos)[i].z);
        buf_ptr += written;
        *length += written;
    }

    // Write texture coordinates (vt) to buffer
    for (saci_u64 i = 0; i < *verticesTexCoordsAmount; ++i) {
        int written = snprintf(buf_ptr, estimated_size - *length, "vt %.2f %.2f\n",
                               (*verticesTexCoords)[i].x, (*verticesTexCoords)[i].y);
        buf_ptr += written;
        *length += written;
    }

    // Write normals (vn) to buffer
    for (saci_u64 i = 0; i < *normalsAmount; ++i) {
        int written = snprintf(buf_ptr, estimated_size - *length, "vn %.2f %.2f %.2f\n",
                               (*normals)[i].x, (*normals)[i].y, (*normals)[i].z);
        buf_ptr += written;
        *length += written;
    }

    // Write faces (f) to buffer
    for (saci_u64 i = 0; i < *indicesAmount; ++i) {
        int written = snprintf(
            buf_ptr, estimated_size - *length, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
            (*indices)[i].vertexIndex, (*indices)[i].texCoordIndex, (*indices)[i].normalIndex,
            (*indices)[i].vertexIndex, (*indices)[i].texCoordIndex, (*indices)[i].normalIndex,
            (*indices)[i].vertexIndex, (*indices)[i].texCoordIndex, (*indices)[i].normalIndex);
        buf_ptr += written;
        *length += written;
    }

    // Reallocate buffer to the exact size used
    *buffer = (char*)realloc(*buffer, *length + 1);
    (*buffer)[*length] = '\0'; // Null-terminate the buffer
}

/**
 * @brief
 *
 * @param buffer
 * @param length
 * @param verticesPos
 * @param verticesAmount
 * @param verticesTexCoords
 * @param verticesTexCoordsAmount
 * @param normals
 * @param normalsAmount
 * @param sc_VertexIndice
 * @param indices
 * @param indicesAmount
 * @param generator
 */
static void __generateRandomBufferWithNonTriangulatedFaces(
    char** buffer, saci_u64* length, saci_Vec3** verticesPos, saci_u64* verticesAmount,
    saci_Vec2** verticesTexCoords, saci_u64* verticesTexCoordsAmount, saci_Vec3** normals,
    saci_u64* normalsAmount, struct sc_VertexIndice** indices,
    saci_u64* indicesAmount) { // Seed the random number generator
    srand((unsigned int)time(NULL));

    // Generate random vertices
    *verticesAmount = 10 + rand() % 1000; // random between 10 and 20 vertices
    *verticesPos = (saci_Vec3*)malloc(*verticesAmount * sizeof(saci_Vec3));
    for (saci_u64 i = 0; i < *verticesAmount; ++i) {
        (*verticesPos)[i].x = (float)(rand() % 100) / 10.0f;
        (*verticesPos)[i].y = (float)(rand() % 100) / 10.0f;
        (*verticesPos)[i].z = (float)(rand() % 100) / 10.0f;
    }

    // Generate random texture coordinates
    *verticesTexCoordsAmount = *verticesAmount; // same amount as vertices
    *verticesTexCoords = (saci_Vec2*)malloc(*verticesTexCoordsAmount * sizeof(saci_Vec2));
    for (saci_u64 i = 0; i < *verticesTexCoordsAmount; ++i) {
        (*verticesTexCoords)[i].x = (float)(rand() % 100) / 100.0f;
        (*verticesTexCoords)[i].y = (float)(rand() % 100) / 100.0f;
    }

    // Generate random normals
    *normalsAmount = *verticesAmount; // same amount as vertices
    *normals = (saci_Vec3*)malloc(*normalsAmount * sizeof(saci_Vec3));
    for (saci_u64 i = 0; i < *normalsAmount; ++i) {
        (*normals)[i].x = (float)(rand() % 100) / 10.0f;
        (*normals)[i].y = (float)(rand() % 100) / 10.0f;
        (*normals)[i].z = (float)(rand() % 100) / 10.0f;
    }

    // Generate random indices for faces (non-triangulated faces, e.g., quadrilaterals)
    *indicesAmount = (*verticesAmount >= 4) ? (*verticesAmount / 2)
                                            : 0; // Create quadrilaterals if enough vertices exist
    *indices = (struct sc_VertexIndice*)malloc(*indicesAmount * sizeof(struct sc_VertexIndice) *
                                               4); // 4 vertices per face
    for (saci_u64 i = 0; i < *indicesAmount; ++i) {
        (*indices)[i * 4 + 0].vertexIndex = (rand() % *verticesAmount) + 1;
        (*indices)[i * 4 + 1].vertexIndex = (rand() % *verticesAmount) + 1;
        (*indices)[i * 4 + 2].vertexIndex = (rand() % *verticesAmount) + 1;
        (*indices)[i * 4 + 3].vertexIndex = (rand() % *verticesAmount) + 1;

        (*indices)[i * 4 + 0].texCoordIndex = (rand() % *verticesTexCoordsAmount) + 1;
        (*indices)[i * 4 + 1].texCoordIndex = (rand() % *verticesTexCoordsAmount) + 1;
        (*indices)[i * 4 + 2].texCoordIndex = (rand() % *verticesTexCoordsAmount) + 1;
        (*indices)[i * 4 + 3].texCoordIndex = (rand() % *verticesTexCoordsAmount) + 1;

        (*indices)[i * 4 + 0].normalIndex = (rand() % *normalsAmount) + 1;
        (*indices)[i * 4 + 1].normalIndex = (rand() % *normalsAmount) + 1;
        (*indices)[i * 4 + 2].normalIndex = (rand() % *normalsAmount) + 1;
        (*indices)[i * 4 + 3].normalIndex = (rand() % *normalsAmount) + 1;
    }

    // Calculate buffer size and prepare for OBJ format
    *length = 0;
    saci_u64 estimated_size =
        256 * (*verticesAmount + *verticesTexCoordsAmount + *normalsAmount + *indicesAmount);
    *buffer = (char*)malloc(estimated_size);
    char* buf_ptr = *buffer;

    // Write vertices (v) to buffer
    for (saci_u64 i = 0; i < *verticesAmount; ++i) {
        int written = snprintf(buf_ptr, estimated_size - *length, "v %.2f %.2f %.2f\n",
                               (*verticesPos)[i].x, (*verticesPos)[i].y, (*verticesPos)[i].z);
        buf_ptr += written;
        *length += written;
    }

    // Write texture coordinates (vt) to buffer
    for (saci_u64 i = 0; i < *verticesTexCoordsAmount; ++i) {
        int written = snprintf(buf_ptr, estimated_size - *length, "vt %.2f %.2f\n",
                               (*verticesTexCoords)[i].x, (*verticesTexCoords)[i].y);
        buf_ptr += written;
        *length += written;
    }

    // Write normals (vn) to buffer
    for (saci_u64 i = 0; i < *normalsAmount; ++i) {
        int written = snprintf(buf_ptr, estimated_size - *length, "vn %.2f %.2f %.2f\n",
                               (*normals)[i].x, (*normals)[i].y, (*normals)[i].z);
        buf_ptr += written;
        *length += written;
    }

    // Write non-triangulated faces (f) to buffer
    for (saci_u64 i = 0; i < *indicesAmount; ++i) {
        int written =
            snprintf(buf_ptr, estimated_size - *length, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
                     (*indices)[i * 4 + 0].vertexIndex, (*indices)[i * 4 + 0].texCoordIndex,
                     (*indices)[i * 4 + 0].normalIndex, (*indices)[i * 4 + 1].vertexIndex,
                     (*indices)[i * 4 + 1].texCoordIndex, (*indices)[i * 4 + 1].normalIndex,
                     (*indices)[i * 4 + 2].vertexIndex, (*indices)[i * 4 + 2].texCoordIndex,
                     (*indices)[i * 4 + 2].normalIndex, (*indices)[i * 4 + 3].vertexIndex,
                     (*indices)[i * 4 + 3].texCoordIndex, (*indices)[i * 4 + 3].normalIndex);
        buf_ptr += written;
        *length += written;
    }

    // Reallocate buffer to the exact size used
    *buffer = (char*)realloc(*buffer, *length + 1);
    (*buffer)[*length] = '\0'; // Null-terminate the buffer
}

static void TestOBJParse(void);

void saci_TestModelLoading(void) { TestOBJParse(); }

static void TestOBJParse(void) {
    // Base values
    char* buffer;
    saci_u64 length;
    saci_Vec3* verticesPosExpected;
    saci_u64 verticesAmountExpected;
    saci_Vec2* verticesTexCoordsExpected;
    saci_u64 verticesTexCoordsAmountExpected;
    saci_Vec3* normalsExpected;
    saci_u64 normalsAmountExpected;
    struct sc_VertexIndice* indicesExpected;
    saci_u64 indicesAmountExpected;

    {
        saci_Test_AddDescription("Should load the information correctly for triangulated faces");
        __generateRandomBuffer(&buffer, &length, &verticesPosExpected, &verticesAmountExpected,
                               &verticesTexCoordsExpected, &verticesTexCoordsAmountExpected,
                               &normalsExpected, &normalsAmountExpected, &indicesExpected,
                               &indicesAmountExpected);

        saci_Vec3* verticesPos;
        saci_u64 verticesPosAmount;
        saci_Vec2* verticesTexCoords;
        saci_u64 verticesTexCoordsAmount;
        struct sc_VertexIndice* indices;
        saci_u64 indicesAmount;

        saci_Test_ClockBegin();
        // OBJ_Parse Function
        sc_OBJ_Parse(buffer, length, &verticesPos, &verticesPosAmount, &verticesTexCoords,
                     &verticesTexCoordsAmount, &indices, &indicesAmount);
        saci_Test_ClockEnd();
        SACI_TEST_ASSERT(saci_GetElapsedTimeMS() < 50); // should load under 50 ms

        SACI_TEST_ASSERT(verticesPosAmount == verticesAmountExpected);
        for (saci_u64 i = 0; i < verticesPosAmount; ++i) {
            SACI_TEST_ASSERT(SACI_TEST_VEC3_IS_EQUAL(verticesPosExpected[i], verticesPos[i]));
        }
        SACI_TEST_ASSERT(verticesTexCoordsAmount == verticesTexCoordsAmountExpected);
        for (saci_u64 i = 0; i < verticesTexCoordsAmount; ++i) {
            SACI_TEST_ASSERT(
                SACI_TEST_VEC2_IS_EQUAL(verticesTexCoords[i], verticesTexCoordsExpected[i]));
        }
        SACI_TEST_ASSERT(indicesAmount == indicesAmountExpected);
        for (saci_u64 i = 0; i < indicesAmount; ++i) {
            SACI_TEST_ASSERT(indices[i].vertexIndex == indicesExpected[i].vertexIndex);
            SACI_TEST_ASSERT(indices[i].texCoordIndex == indicesExpected[i].texCoordIndex);
            SACI_TEST_ASSERT(indices[i].normalIndex == indicesExpected[i].normalIndex);
        }
        free(verticesPos);
        free(verticesTexCoords);
        free(indices);
    }
    {
        saci_Test_AddDescription(
            "Should load the information correctly for NON-TRIANGULATED faces");
        __generateRandomBufferWithNonTriangulatedFaces(
            &buffer, &length, &verticesPosExpected, &verticesAmountExpected,
            &verticesTexCoordsExpected, &verticesTexCoordsAmountExpected, &normalsExpected,
            &normalsAmountExpected, &indicesExpected, &indicesAmountExpected);

        saci_Vec3* verticesPos;
        saci_u64 verticesPosAmount;
        saci_Vec2* verticesTexCoords;
        saci_u64 verticesTexCoordsAmount;
        struct sc_VertexIndice* indices;
        saci_u64 indicesAmount;
        sc_OBJ_Parse(buffer, length, &verticesPos, &verticesPosAmount, &verticesTexCoords,
                     &verticesTexCoordsAmount, &indices, &indicesAmount);

        SACI_TEST_ASSERT(verticesPosAmount == verticesAmountExpected);
        for (saci_u64 i = 0; i < verticesPosAmount; ++i) {
            SACI_TEST_ASSERT(SACI_TEST_VEC3_IS_EQUAL(verticesPosExpected[i], verticesPos[i]));
        }
        SACI_TEST_ASSERT(verticesTexCoordsAmount == verticesTexCoordsAmountExpected);
        for (saci_u64 i = 0; i < verticesTexCoordsAmount; ++i) {
            SACI_TEST_ASSERT(
                SACI_TEST_VEC2_IS_EQUAL(verticesTexCoords[i], verticesTexCoordsExpected[i]));
        }
        SACI_TEST_ASSERT(indicesAmount == indicesAmountExpected);
        for (saci_u64 i = 0; i < indicesAmount; ++i) {
            SACI_TEST_ASSERT(indices[i].vertexIndex == indicesExpected[i].vertexIndex);
            SACI_TEST_ASSERT(indices[i].texCoordIndex == indicesExpected[i].texCoordIndex);
            SACI_TEST_ASSERT(indices[i].normalIndex == indicesExpected[i].normalIndex);
        }
        free(verticesPos);
        free(verticesTexCoords);
        free(indices);
    }
}
