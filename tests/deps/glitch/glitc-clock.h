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

#include <time.h>

GLITCH_DEF void   glitch_Clock_Begin(void);
GLITCH_DEF void   glitch_Clock_End(void);
GLITCH_DEF double glitch_Clock_Get_Elapsed_MS(void);

/* === GLITCH_CLOCK Implementation === */

#ifdef GLITCH_CLOCK

GLITCH_DEF clock_t __glitch_time_start = 0;
GLITCH_DEF clock_t __glitch_time_end   = 0;

GLITCH_IMPL void glitch_Clock_Begin(void) {
    __glitch_time_start = clock();
}

GLITCH_IMPL void glitch_Clock_End(void) {
    __glitch_time_end = clock();
}

GLITCH_IMPL double glitch_Clock_Get_Elapsed_MS(void) {
    return (double)(1000.0f * (__glitch_time_end - __glitch_time_start) / CLOCKS_PER_SEC);
}

#endif // GLITCH_CLOCK
