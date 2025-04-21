#ifndef GLITCH_DEF

#ifdef _WIN32
 #ifdef BUILD_GLITCH_LIB
  #define GLITCH_DEF __declspec(dllexport) // Export symbols when building the library
 #else
  #define GLITCH_DEF __declspec(dllimport) // Import symbols when using the library
 #endif
#else
 #define GLITCH_DEF // Non-Windows platforms don't need special decoration
#endif // _WIN32

#endif // GLITCH_DEF

#ifndef GLITCH_IMPL
#define GLITCH_IMPL inline GLITCH_DEF
#endif // GLITCH_IMPL

#include <math.h>
#include <stdlib.h>

GLITCH_DEF int   glitch_Rand_Range_Int(int min, int max);
GLITCH_DEF float glitch_Rand_Range_Float(float min, float max);

/* === GLITCH_RAND Implementation === */
#ifdef GLITCH_RAND

GLITCH_IMPL int glitch_Rand_Range_Int(int min, int max) {
    return ((rand() % ((max) - (min) + 1)) + (min));
}

GLITCH_IMPL float glitch_Rand_Range_Float(float min, float max) {
    return ((min) + ((float)rand() / (float)RAND_MAX) * ((max) - (min)));
}

#endif // GLITCH_RAND
