#include <stdlib.h>
#include <assert.h>

#include "saci-core/sc-gl.h"
#include "saci-testing.h"
#include "saci-utils/su-types.h"

static sa_vec3_t __RandVec3(void) {
    float x = SACI_RAND_RANGE_FLOAT(5, true),
          y = SACI_RAND_RANGE_FLOAT(5, true),
          z = SACI_RAND_RANGE_FLOAT(5, true);
    return (sa_vec3_t){x, y, z};
}

static sa_color_t __RandColor(void) {
    float r = SACI_RAND_RANGE_FLOAT(1, false),
          g = SACI_RAND_RANGE_FLOAT(1, false),
          b = SACI_RAND_RANGE_FLOAT(1, false),
          a = SACI_RAND_RANGE_FLOAT(1, false);
    return (sa_color_t){r, g, b, a};
}

static sa_vec2_t __RandVec2(void) {
    float x = SACI_RAND_RANGE_FLOAT(5, true),
          y = SACI_RAND_RANGE_FLOAT(5, true);
    return (sa_vec2_t){x, y};
}

static void saci_Test_Vertices(void);

void saci_TestRendering(void) { saci_Test_Vertices(); }

static void saci_Test_Vertices(void) {
    { /** === Creating vertices === **/
        sa_vec3_t position = __RandVec3();
        sa_color_t color = __RandColor();
        sa_vec2_t texcoord = __RandVec2();
        struct sc_vertice_c *vertice =
            sc_Vertice_Create_Vertice(position, color, texcoord);
        assert(vertice);

        SACI_TEST_ASSERT(
            SACI_TEST_VEC3_IS_EQUAL(position, sc_Vertice_Get_Pos(vertice)),
            "Should have correct indices");
        SACI_TEST_ASSERT(
            SACI_TEST_VEC2_IS_EQUAL(texcoord, sc_Vertice_Get_Texcoord(vertice)),
            "Should have correct texcoords");
        SACI_TEST_ASSERT(
            SACI_TEST_COLOR_IS_EQUAL(color, sc_Vertice_Get_Color(vertice)),
            "Should have correct colors");
        free(vertice);
    }
    { /** === Creating vertice array === **/
        sa_vec3_t positionsExpected[4] = {__RandVec3(), __RandVec3(),
                                          __RandVec3(), __RandVec3()};
        sa_color_t colorsExpected[4] = {__RandColor(), __RandColor(),
                                        __RandColor(), __RandColor()};
        sa_vec2_t texcoordsExpected[4] = {__RandVec2(), __RandVec2(),
                                          __RandVec2(), __RandVec2()};
        struct sc_vertice_c *vertices = sc_Vertice_Create_Vertices_Array(
            positionsExpected, colorsExpected, texcoordsExpected, 4);
        assert(vertices);

        sa_vec3_t *positions;
        sa_color_t *colors;
        sa_vec2_t *texcoords;
        sc_Vertice_Get_Array_Info(vertices, 4, &positions, &colors, &texcoords);

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
