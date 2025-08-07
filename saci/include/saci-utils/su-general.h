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

#include "saci-utils/su-types.h"

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

#ifndef SA_INTERNAL_CONST
#  define SA_INTERNAL_CONST static const
#endif // SA_INTERNAL

#ifndef SA_INTERNAL_INLINE
#  ifndef SA_INTERNAL
#    define SA_INTERNAL_INLINE SA_INTERNAL inline
#  else
#    define SA_INTERNAL_INLINE SA_INTERNAL
#  endif
#endif // SA_INTERNAL_INLINE

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
#ifndef su_SCAST_TO_M
#  ifdef __cplusplus
#    define su_SCAST_TO_M(type) static_cast<type>
#  else
#    define su_SCAST_TO_M(type) (type)
#  endif // __cplusplus
#endif   // su_SCAST_TO_M

/**
 * @define su_MALLOC_M
 * @brief `malloc` version of saciGL
 *
 * Feel free to overwrite it with your own! When in SACI_DEBUG_MODE will print
 * the origin of the allocated buffer
 *
 * @param[in] x The size of the buffer to be alloced
 *
 * @return The allocated buffer
 */
#ifndef su_MALLOC_M
#  include <stdlib.h>
#  include <stdio.h>

#  ifdef SACI_DEBUG_MODE
#    define su_MALLOC_M(x)                                       \
        ({                                                       \
        size_t _size = (x);                                      \
        void* _ptr = malloc(_size);                              \
        printf("allocating %lu bytes in func: %s at line: %d\n", \
               (unsigned long)_size, __func__, __LINE__);        \
        _ptr;                                                    \
        })
#  else
#    define su_MALLOC_M(x) malloc(x)
#  endif

#endif // su_MALLOC_M

/**
 * @define su_CALLOC_M
 * @brief `calloc` version of saciGL
 *
 * Feel free to overwrite it with your own! When in SACI_DEBUG_MODE will print
 * the origin of the allocated buffer
 *
 * @param[in] n The number of elements to be alloced
 * @param[in] x The size of the elements to be alloced
 *
 * @return The allocated buffer
 */
#ifndef su_CALLOC_M
#  include <stdlib.h>
#  include <stdio.h>

#  ifdef SACI_DEBUG_MODE
#    define su_CALLOC_M(n, x)                                    \
        ({                                                       \
        size_t _size = ((n) * (x));                              \
        void* _ptr = calloc(n, x);                               \
        printf("allocating %lu bytes in func: %s at line: %d\n", \
               (unsigned long)_size, __func__, __LINE__);        \
        _ptr;                                                    \
        })
#  else
#    define su_CALLOC_M(n, x) calloc(n, x)
#  endif

#endif // su_CALLOC_M

/**
 * @define su_FREE_M
 * @brief `free` version of saciGL
 *
 * Feel free to overwrite it with your own! When in SACI_DEBUG_MODE will print
 * the origin of the freed buffer
 *
 * @param[in] x The buffer to be freed
 */
#ifndef su_FREE_M
#  include <stdlib.h>
#  ifdef SACI_DEBUG_MODE
#    define su_FREE_M(x)                                                                  \
        ({                                                                                \
        if (x) {                                                                          \
            printf("freeing %p in func: %s at line: %d\n", (void*)x, __func__, __LINE__); \
            free(x);                                                                      \
            x = NULL;                                                                     \
        } else {                                                                          \
            printf("trying to free invalid pointer at: %s, %d", __func__, __LINE__);      \
        }                                                                                 \
        })
#  else
#    define su_FREE_M(x) free(x)
#  endif // SACI_DEBUG_MODE
#endif   // su_FREE_M

/**
 * @define su_NOT_USED_M
 * @brief Removes a warning from a not used var when in debug mode
 *
 * When not in debug mode, generates a message: "Warning: su_NOT_USED_M is used —
 * remove before release!"
 * @param[in] x The variable to have it's not used warning suppresed
 */
#ifndef su_NOT_USED_M
#  ifdef SACI_DEBUG_MODE

#    define su_NOT_USED_M(x) (void)(x) // Or leave it undefined to cause a build error
#  else
#    define su_NOT_USED_M(x) \
        _Pragma("message(\"Warning: su_NOT_USED_M is used — remove before release!\")")(void)(x)

#  endif // SACI_DEBUG_MODE
#endif   // su_NOT_USED_M

/**
 * @define su_MIN_M
 * @brief Returns the smallest of two values
 *
 * @param[in] x The smallest value
 */
#ifndef su_MIN_M
#  define su_MIN_M(x, y) ((x) < (y) ? (x) : (y))
#endif // su_MIN_M

/**
 * @define su_MAX_M
 * @brief Returns the biggest of two values
 *
 * @param[in] x The biggest value
 */
#ifndef su_MAX_M
#  define su_MAX_M(x, y) ((x) > (y) ? (x) : (y))
#endif // su_MAX_M

typedef struct su_DArray su_DArray;

#define su_SIZE_OF_DARRAY 40

SA_API su_DArray* su_darray_create(su_U64 capacity, su_U64 elem_size, su_Bool fixed_size);

SA_API su_DArray* su_darray_create_ctx(void* memctx, su_U64 memctx_size, su_U64 capacity, su_U64 elem_size, su_Bool fixed_size);

SA_API void su_darray_free(su_DArray* array);

SA_API void su_darray_clear(su_DArray* array);

SA_API su_Bool su_darray_resize(su_DArray* array, su_U64 new_cap);

SA_API su_Bool su_darray_push(su_DArray* array, const void* value);

SA_API void su_darray_pop(su_DArray* array);

SA_API void su_darray_get(const su_DArray* array, su_U64 index, void* out_value);

SA_API void* su_darray_get_ptr(const su_DArray* array, su_U64 index);

SA_API void su_darray_set(su_DArray* array, su_U64 index, const void* value);

SA_API su_U64 su_darray_length(const su_DArray* array);

SA_API su_U64 su_darray_capacity(const su_DArray* array);

SA_API void su_darray_append(su_DArray* dest, const su_DArray* src);

SA_API void su_darray_debug_print(const su_DArray* arr);

SA_API su_Bool su_darray_is_null(const su_DArray* arr);

SA_API su_Bool su_darray_is_empty(const su_DArray* arr);

#endif // SACI_UTILS_SU_GENERAL_H
