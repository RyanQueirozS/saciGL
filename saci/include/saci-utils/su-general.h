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
#ifndef su_Scast_To_m
#  ifdef __cplusplus
#    define su_Scast_To_m(type) static_cast<type>
#  else
#    define su_Scast_To_m(type) (type)
#  endif // __cplusplus
#endif   // su_Scast_To_m

/**
 * @define su_Malloc_m
 * @brief `malloc` version of saciGL
 *
 * Feel free to overwrite it with your own! When in SACI_DEBUG_MODE will print
 * the origin of the allocated buffer
 *
 * @param[in] x The size of the buffer to be alloced
 *
 * @return The allocated buffer
 */
#ifndef su_Malloc_m
#  include <stdlib.h>
#  include <stdio.h>

#  ifdef SACI_DEBUG_MODE
#    define su_Malloc_m(x)                                       \
        ({                                                       \
        size_t _size = (x);                                      \
        void* _ptr = malloc(_size);                              \
        printf("allocating %lu bytes in func: %s at line: %d\n", \
               (unsigned long)_size, __func__, __LINE__);        \
        _ptr;                                                    \
        })
#  else
#    define su_Malloc_m(x) malloc(x)
#  endif

#endif // su_Malloc_m

/**
 * @define su_Calloc_m
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
#ifndef su_Calloc_m
#  include <stdlib.h>
#  include <stdio.h>

#  ifdef SACI_DEBUG_MODE
#    define su_Calloc_m(n, x)                                    \
        ({                                                       \
        size_t _size = ((n) * (x));                              \
        void* _ptr = calloc(n, x);                               \
        printf("allocating %lu bytes in func: %s at line: %d\n", \
               (unsigned long)_size, __func__, __LINE__);        \
        _ptr;                                                    \
        })
#  else
#    define su_Calloc_m(n, x) calloc(n, x)
#  endif

#endif // su_Calloc_m

/**
 * @define su_Free_m
 * @brief `free` version of saciGL
 *
 * Feel free to overwrite it with your own! When in SACI_DEBUG_MODE will print
 * the origin of the freed buffer
 *
 * @param[in] x The buffer to be freed
 */
#ifndef su_Free_m
#  include <stdlib.h>
#  ifdef SACI_DEBUG_MODE
#    define su_Free_m(x)                                                                  \
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
#    define su_Free_m(x) free(x)
#  endif // SACI_DEBUG_MODE
#endif   // su_Free_m

/**
 * @define su_Not_Used_m
 * @brief Removes a warning from a not used var when in debug mode
 *
 * When not in debug mode, generates a message: "Warning: su_Not_Used_m is used —
 * remove before release!"
 * @param[in] x The variable to have it's not used warning suppresed
 */
#ifndef su_Not_Used_m
#  ifdef SACI_DEBUG_MODE

#    define su_Not_Used_m(x) (void)(x) // Or leave it undefined to cause a build error
#  else
#    define su_Not_Used_m(x) \
        _Pragma("message(\"Warning: su_Not_Used_m is used — remove before release!\")")(void)(x)

#  endif // SACI_DEBUG_MODE
#endif   // su_Not_Used_m

/**
 * @define su_Min_m
 * @brief Returns the smallest of two values
 *
 * @param[in] x The smallest value
 */
#ifndef su_Min_m
#  define su_Min_m(x, y) ((x) < (y) ? (x) : (y))
#endif // su_Min_m

/**
 * @define su_Max_m
 * @brief Returns the biggest of two values
 *
 * @param[in] x The biggest value
 */
#ifndef su_Max_m
#  define su_Max_m(x, y) ((x) > (y) ? (x) : (y))
#endif // su_Max_m

typedef struct su_dArray su_dArray;

#define SIZE_OF_DARRAY 40

SA_API su_dArray* su_DArray_Create(sa_u64 capacity, sa_u64 elem_size, sa_bool fixed_size);

SA_API su_dArray* su_DArray_Create_Ctx(void* memctx, sa_u64 memctx_size, sa_u64 capacity, sa_u64 elem_size, sa_bool fixed_size);

SA_API void su_DArray_Free(su_dArray* array);

SA_API void su_DArray_Clear(su_dArray* array);

SA_API sa_bool su_DArray_Resize(su_dArray* array, sa_u64 new_cap);

SA_API sa_bool su_DArray_Push(su_dArray* array, const void* value);

SA_API void su_DArray_Pop(su_dArray* array);

SA_API void su_DArray_Get(const su_dArray* array, sa_u64 index, void* out_value);

SA_API void* su_DArray_Get_Ptr(const su_dArray* array, sa_u64 index);

SA_API void su_DArray_Set(su_dArray* array, sa_u64 index, const void* value);

SA_API sa_u64 su_DArray_Length(const su_dArray* array);

SA_API sa_u64 su_DArray_Capacity(const su_dArray* array);

SA_API void su_DArray_Append(su_dArray* dest, const su_dArray* src);

SA_API void su_DArray_Debug_Print(const su_dArray* arr);

SA_API sa_bool su_DArray_Is_Null(const su_dArray* arr);

SA_API sa_bool su_DArray_Is_Empty(const su_dArray* arr);

#endif // SACI_UTILS_SU_GENERAL_H
