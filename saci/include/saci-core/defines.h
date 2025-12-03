#ifndef SACI_CORE_DEFINES_H
#define SACI_CORE_DEFINES_H

#ifndef SA_API

#  ifdef __EMSCRIPTEN__
#    include <emscripten.h>
#    define SA_API EMSCRIPTEN_KEEPALIVE

#  elif defined(_WIN32)
#    ifdef BUILD_SACI_LIB
#      define SA_API __declspec(dllexport) // Export symbols when building the library
#    else
#      define SA_API __declspec(dllimport) // Import symbols when using the library
#    endif

#  else
#    define SA_API // Non-Windows platforms don't need special decoration

#  endif // Platform checks

#endif // SA_API

#ifdef SACI_TEST_BUILD

#  define SA_INTERNAL
#else
#  define SA_INTERNAL static

#endif // SA_TEST_BUILD

#define SA_STATIC static

#define SA_INTERNAL_CONST static const

#ifndef SA_INTERNAL
#  define SA_INTERNAL_INLINE SA_INTERNAL inline
#else
#  define SA_INTERNAL_INLINE SA_INTERNAL
#endif

#endif // SACI_CORE_DEFINES_H
