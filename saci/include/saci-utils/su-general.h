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

#ifndef sa_SCAST_TO_m
#  ifdef __cplusplus
#    define sa_SCAST_TO_m(type) static_cast<type>
#  else
#    define sa_SCAST_TO_m(type) (type)
#  endif
#endif // sa_SCAST_TO_m

#define sa_ARRLEN_m(array) (sizeof(array) / sizeof(array[0]))

#ifndef sa_MALLOC
#  include <stdlib.h>
#  include <stdio.h>

#  ifdef SACI_DEBUG_MODE
#    define sa_MALLOC(x)                             \
        ({                                           \
        size_t _size = (x);                          \
        void* _ptr = malloc(_size);                  \
        printf("allocating %lu bytes in func: %s\n", \
               (unsigned long)_size, __func__);      \
        _ptr;                                        \
        })
#  else
#    define sa_MALLOC(x) malloc(x)
#  endif

#endif // sa_MALLOC

#ifndef sa_FREE
#  include <stdlib.h>
#  define sa_FREE free
#endif // sa_FREE

#ifndef sa_NOT_USED
#  define sa_NOT_USED (void)
#endif // sa_NOT_USED

#ifndef sa_MIN
#  define sa_MIN(x, y) ((x) < (y) ? (x) : (y))
#endif // sa_MIN

#ifndef sa_MAX
#  define sa_MAX(x, y) ((x) > (y) ? (x) : (y))
#endif // sa_MIN

#endif
