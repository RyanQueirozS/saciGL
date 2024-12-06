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

#ifdef __cplusplus

#define sa_SCAST_TO_m(type) static_cast<type>
#else
#define sa_SCAST_TO_m(type) (type)
#endif

#define sa_ARRLEN_m(array) (sizeof(array) / sizeof(array[0]))

#ifndef sa_MALLOC
#include <malloc.h>
#define sa_MALLOC malloc
#endif // sa_MALLOC

#ifndef sa_ASSERT
#include <assert.h>
#define sa_ASSERT assert
#endif // sa_ASSERT

#endif
