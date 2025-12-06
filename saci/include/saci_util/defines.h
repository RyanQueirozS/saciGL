#ifndef SACI_CORE_DEFINES_H
#define SACI_CORE_DEFINES_H

#ifndef SACI_API

#  ifdef __EMSCRIPTEN__
#    include <emscripten.h>
#    define SACI_API EMSCRIPTEN_KEEPALIVE

#  elif defined(_WIN32)
#    ifdef BUILD_SACI_LIB
#      define SACI_API __declspec(dllexport) // Export symbols when building the library
#    else
#      define SACI_API __declspec(dllimport) // Import symbols when using the library
#    endif

#  else
#    define SACI_API // Non-Windows platforms don't need special decoration

#  endif // Platform checks

#endif // SACI_API

#define SACI_EXTERN extern

#ifdef SACI_TEST_BUILD

#  define SACI_INTERNAL
#else
#  define SACI_INTERNAL static

#endif // SACI_TEST_BUILD

#define SACI_STATIC static

#define SACI_INTERNAL_CONST static const

#ifndef SACI_INTERNAL
#  define SACI_INTERNAL_INLINE SACI_INTERNAL inline
#else
#  define SACI_INTERNAL_INLINE SACI_INTERNAL
#endif

#endif // SACI_CORE_DEFINES_H
