#include <stdlib.h>
#include <assert.h>

#include "saci-core/sc-gl.h"
#include "saci-testing.h"
#include "saci-utils/su-types.h"

static saci_Vec3 __RandVec3(void) {
    float x = SACI_RAND_RANGE_FLOAT(5, true),
          y = SACI_RAND_RANGE_FLOAT(5, true),
          z = SACI_RAND_RANGE_FLOAT(5, true);
    return (saci_Vec3){x, y, z};
}

static saci_Color __RandColor(void) {
    float r = SACI_RAND_RANGE_FLOAT(1, false),
          g = SACI_RAND_RANGE_FLOAT(1, false),
          b = SACI_RAND_RANGE_FLOAT(1, false),
          a = SACI_RAND_RANGE_FLOAT(1, false);
    return (saci_Color){r, g, b, a};
}

static saci_Vec2 __RandVec2(void) {
    float x = SACI_RAND_RANGE_FLOAT(5, true),
          y = SACI_RAND_RANGE_FLOAT(5, true);
    return (saci_Vec2){x, y};
}

static void saci_Test_Vertices(void);

void saci_TestRendering(void) { saci_Test_Vertices(); }

static void saci_Test_Vertices(void) {
    { /** === Creating vertices === **/
        saci_Vec3 position = __RandVec3();
        saci_Color color = __RandColor();
        saci_Vec2 texcoord = __RandVec2();
        sc_Vertice *vertice =
            sc_Vertice_CreateVertice(position, color, texcoord);
        assert(vertice);

        SACI_TEST_ASSERT(
            SACI_TEST_VEC3_IS_EQUAL(position, sc_Vertice_GetPos(vertice)),
            "Should have correct indices");
        SACI_TEST_ASSERT(
            SACI_TEST_VEC2_IS_EQUAL(texcoord, sc_Vertice_GetTexcoord(vertice)),
            "Should have correct texcoords");
        SACI_TEST_ASSERT(
            SACI_TEST_COLOR_IS_EQUAL(color, sc_Vertice_GetColor(vertice)),
            "Should have correct colors");
        free(vertice);
    }
    { /** === Creating vertice array === **/
        saci_Vec3 positionsExpected[4] = {__RandVec3(), __RandVec3(),
                                          __RandVec3(), __RandVec3()};
        saci_Color colorsExpected[4] = {__RandColor(), __RandColor(),
                                        __RandColor(), __RandColor()};
        saci_Vec2 texcoordsExpected[4] = {__RandVec2(), __RandVec2(),
                                          __RandVec2(), __RandVec2()};
        sc_Vertice *vertices = sc_Vertice_CreateVerticesArray(
            positionsExpected, colorsExpected, texcoordsExpected, 4);
        assert(vertices);

        saci_Vec3 *positions;
        saci_Color *colors;
        saci_Vec2 *texcoords;
        sc_Vertice_GetArrayInfo(vertices, 4, &positions, &colors, &texcoords);

        for (int i = 0; i < 4; ++i) {
            SACI_TEST_ASSERT(
                SACI_TEST_VEC3_IS_EQUAL(positions[i], positionsExpected[i]),
                "Should have correct indices");
            SACI_TEST_ASSERT(
                SACI_TEST_VEC2_IS_EQUAL(texcoords[i], texcoordsExpected[i]),
                "Should have correct texcoords");
            SACI_TEST_ASSERT(
                SACI_TEST_COLOR_IS_EQUAL(colors[i], colorsExpected[i]),
                "Should have correct colors");
        }
        free(vertices);
    }
}
