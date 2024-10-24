/**
 * @file su-general.h
 * @brief This header defines common code utilities used in the saciCORE and saciLIB.
 *
 * @details
 * This file provides general code utilities, like type casting macros and macros to get array lengths
 */

#ifndef __SACI_UTILS_SU_GENERAL_H__
#define __SACI_UTILS_SU_GENERAL_H__

#ifdef __cplusplus
/**
 * @def SACI_PI
 * @brief Casts a type using static_cast from C++
 * @param type Type to cast
 */
#define SACI_SCAST_TO(type) static_cast<type>
#else
/**
 * @def SACI_PI
 * @brief Casts a type using C style casting
 *
 * @param type Type to cast
 */
#define SACI_SCAST_TO(type) (type)
#endif

/**
 * @def SACI_ARRLEN
 * @brief Macro to get array length
 *
 * @param array Array to get length
 * @return The length of the array
 */
#define SACI_ARRLEN(array) (sizeof(array) / sizeof(array[0]))

#endif
