#ifndef GLITCH_DEF
#ifdef _WIN32
#ifdef BUILD_GLITCH_LIB
#define GLITCH_DEF __declspec(dllexport) // Export symbols when building the library
#else
#define GLITCH_DEF __declspec(dllimport) // Import symbols when using the library
#endif
#else
#define GLITCH_DEF // Non-Windows platforms don't need special decoration
#endif             // _WIN32
#endif             // GLITCH_DEF

#ifndef GLITCH_IMPL
#define GLITCH_IMPL inline GLITCH_DEF
#endif // GLITCH_IMPL

#include <stdbool.h>

// You may change the type to int, double or whatever. Just define the macro
// before including with a different type.
// EXAMPLE:
// #define GLITCH_CMPX_VEC2(name) int name[2]
// #include "path-to/glitc-complex.h"
#ifndef GLITCH_CMPX_VEC2
#define GLITCH_CMPX_VEC2(name) float name[2]
#endif // GLITCH_CMPX_VEC2

#ifndef GLITCH_CMPX_VEC3
#define GLITCH_CMPX_VEC3(name) float name[3]
#endif // GLITCH_CMPX_VEC3

#ifndef GLITCH_CMPX_VEC4
#define GLITCH_CMPX_VEC4(name) float name[4]
#endif // GLITCH_CMPX_VEC4

GLITCH_DEF bool glitch_Cmpx_Vec2_Is_Equal(GLITCH_CMPX_VEC2(v1), GLITCH_CMPX_VEC2(v2));
GLITCH_DEF bool glitch_Cmpx_Vec3_Is_Equal(GLITCH_CMPX_VEC3(v1), GLITCH_CMPX_VEC3(v2));
GLITCH_DEF bool glitch_Cmpx_Vec4_Is_Equal(GLITCH_CMPX_VEC4(v1), GLITCH_CMPX_VEC4(v2));

/* === GLITCH_COMPLEX Implementation === */

#ifdef GLITCH_COMPLEX

GLITCH_IMPL bool glitch_Cmpx_Vec2_Is_Equal(GLITCH_CMPX_VEC2(v1), GLITCH_CMPX_VEC2(v2)) {
    bool matches = true;
    for (int i = 0; i < 2; ++i) {
        if (v1[i] != v2[i])
            matches = false;
    }
    return matches;
}

GLITCH_IMPL bool glitch_Cmpx_Vec3_Is_Equal(GLITCH_CMPX_VEC3(v1), GLITCH_CMPX_VEC3(v2)) {
    bool matches = true;
    for (int i = 0; i < 3; ++i) {
        if (v1[i] != v2[i])
            matches = false;
    }
    return matches;
}

GLITCH_IMPL bool glitch_Cmpx_Vec4_Is_Equal(GLITCH_CMPX_VEC4(v1), GLITCH_CMPX_VEC4(v2)) {
    bool matches = true;
    for (int i = 0; i < 4; ++i) {
        if (v1[i] != v2[i])
            matches = false;
    }
    return matches;
}

#endif // GLITCH_COMPLEX
