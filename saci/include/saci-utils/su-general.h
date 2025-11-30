/**
 * @file su-general.h
 * @brief This header defines common code utilities used in the saciCORE and saciLIB.
 *
 * @details
 * This file provides general code utilities, like type casting macros and macros to get array
 * lengths
 */

#ifndef SACI_UTILS_SU_GENERAL_H
#define SACI_UTILS_SU_GENERAL_H

#include <stdlib.h>

#ifndef SACI_LIB_PATH
#  error "SACI_LIB_PATH not defined! Set it in CMakeLists.txt or config.sh"
#endif

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

/**
 * @define su_Scast_To_m
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
#ifdef __cplusplus
#  define su_SCAST_TO_M(type) static_cast<type>
#else
#  define su_CAST_M(type) (type)
#endif // __cplusplus

#define su_ARRLEN_M(ar) sizeof(ar) / sizeof((ar)[0])

/**
 * @define su_NOT_USED_M
 * @brief Removes a warning from a not used var when in debug mode
 *
 * When not in debug mode, generates a message: "Warning: su_NOT_USED_M is used —
 * remove before release!"
 * @param[in] x The variable to have it's not used warning suppresed
 */
#ifdef SACI_DEBUG_MODE

#  define su_NOT_USED_M(x) (void)(x)
#else
#  define su_NOT_USED_M(x) \
      _Pragma("message(\"Warning: su_NOT_USED_M is used — remove before release!\")")(void)(x)

#endif // SACI_DEBUG_MODE

#ifdef SACI_DEBUG_MODE
#  define su_TODO_M \
      printf("Not implemented %s:%d\n", __FILE__, __LINE__)
#else
#  define su_TODO_M                                          \
      printf("Not implemented %s:%d\n", __FILE__, __LINE__); \
      abort()
#endif // SACI_DEBUG_MODE

/**
 * @define su_MIN_M
 * @brief Returns the smallest of two values
 *
 * @param[in] x The smallest value
 */
#define su_MIN_M(x, y) ((x) < (y) ? (x) : (y))

/**
 * @define su_MAX_M
 * @brief Returns the biggest of two values
 *
 * @param[in] x The biggest value
 */
#define su_MAX_M(x, y) ((x) > (y) ? (x) : (y))

#ifndef SACI_MAX_TEXTURES
#  define SACI_MAX_TEXTURES 16
#endif // SACI_MAX_TEXTURES

#define su_TEXTURE_INVALID (UINT32_MAX)

#define su_HAS_FLAG(var, flag) (((var) & (flag)) == (flag))

#define su_ADD_FLAG(var, flag) ((var) |= (flag))

#define su_REMOVE_FLAG(var, flag) ((var) &= ~(flag))

#define su_TOGGLE_FLAG(var, flag) ((var) ^= (flag))

#endif // SACI_UTILS_SU_GENERAL_H
