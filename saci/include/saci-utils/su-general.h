/**
 * @file su-general.h
 * @brief This header defines common code utilities used in the saciCORE and saciLIB.
 *
 * @details
 * This file provides general code utilities, like type casting macros and macros to get array
 * lengths
 */

#ifndef __SACI_UTILS_SU_GENERAL_H__
#define __SACI_UTILS_SU_GENERAL_H__

#include <saci-utils/su-types.h>

#ifndef SA_API

#  ifdef _WIN32

#    ifdef BUILD_SACI_LIB
#      define SA_API __declspec(dllexport) // Export symbols when building the library
#    else
#      define SA_API __declspec(dllimport) // Import symbols when using the library
#    endif
#  else
#    define SA_API // Non-Windows platforms don't need special decoration

#  endif // _WIN32

#endif // SA_API

#ifndef SA_INTERNAL
#  ifdef SACI_TEST_BUILD

#    define SA_INTERNAL
#  else
#    define SA_INTERNAL static

#  endif // SA_TEST_BUILD
#endif   // SA_INTERNAL

/**
 * @define sa_Scast_To_m
 * @brief Performs a type-safe cast depending on the language context (C or C++).
 *
 * This macro abstracts type casting in a cross-language (C/C++) environment.
 * In C++, it uses `static_cast<type>` for safer casting. In C, it falls back
 * to a traditional C-style cast `(type)`.
 *
 * @param[in] type The target type to cast to.
 *
 * @return The expression with the applied cast. In C++, uses `static_cast<type>`,
 *         and in C, uses `(type)`.
 */
#ifndef sa_Scast_To_m
#  ifdef __cplusplus
#    define sa_Scast_To_m(type) static_cast<type>
#  else
#    define sa_Scast_To_m(type) (type)
#  endif // __cplusplus
#endif   // sa_Scast_To_m

/**
 * @define sa_Malloc_m
 * @brief `malloc` version of saciGL
 *
 * Feel free to overwrite it with your own! When in SACI_DEBUG_MODE will print
 * the origin of the allocated buffer
 *
 * @param[in] x The size of the buffer to be alloced
 *
 * @return The allocated buffer
 */
#ifndef sa_Malloc_m
#  include <stdlib.h>
#  include <stdio.h>

#  ifdef SACI_DEBUG_MODE
#    define sa_Malloc_m(x)                           \
        ({                                           \
        size_t _size = (x);                          \
        void* _ptr = malloc(_size);                  \
        printf("allocating %lu bytes in func: %s\n", \
               (unsigned long)_size, __func__);      \
        _ptr;                                        \
        })
#  else
#    define sa_Malloc_m(x) malloc(x)
#  endif

#endif // sa_Malloc_m

/**
 * @define sa_Free_m
 * @brief `free` version of saciGL
 *
 * Feel free to overwrite it with your own! When in SACI_DEBUG_MODE will print
 * the origin of the freed buffer
 *
 * @param[in] x The buffer to be freed
 */
#ifndef sa_Free_m
#  include <stdlib.h>
#  define sa_Free_m(x) free(x)
#endif // sa_Free_m

/**
 * @define sa_Not_Used_m
 * @brief Removes a warning from a not used var when in debug mode
 *
 * When not in debug mode, generates a message: "Warning: sa_Not_Used_m is used —
 * remove before release!"
 * @param[in] x The variable to have it's not used warning suppresed
 */
#ifndef sa_Not_Used_m
#  ifdef SACI_DEBUG_MODE

#    define sa_Not_Used_m(x) (void)(x) // Or leave it undefined to cause a build error
#  else
#    define sa_Not_Used_m(x) \
        _Pragma("message(\"Warning: sa_Not_Used_m is used — remove before release!\")")(void)(x)

#  endif // SACI_DEBUG_MODE
#endif   // sa_Not_Used_m

/**
 * @define sa_Min_m
 * @brief Returns the smallest of two values
 *
 * @param[in] x The smallest value
 */
#ifndef sa_Min_m
#  define sa_Min_m(x, y) ((x) < (y) ? (x) : (y))
#endif // sa_Min_m

/**
 * @define sa_Max_m
 * @brief Returns the biggest of two values
 *
 * @param[in] x The biggest value
 */
#ifndef sa_Max_m
#  define sa_Max_m(x, y) ((x) > (y) ? (x) : (y))
#endif // sa_Max_m

#endif
