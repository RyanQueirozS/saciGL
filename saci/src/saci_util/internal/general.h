#ifndef SACI_UTILS_SU_GENERAL_H
#define SACI_UTILS_SU_GENERAL_H

#include <stdlib.h>

#ifndef SACI_LIB_PATH
#  error "SACI_LIB_PATH not defined! Set it in CMakeLists.txt or config.sh"
#endif

#ifdef __cplusplus
#  define SACI_CAST_M(type) static_cast<type>
#else
#  define SACI_CAST_M(type) (type)
#endif // __cplusplus

#define SACI_ARRLEN_M(ar) sizeof(ar) / sizeof((ar)[0])

/**
 * @define SACI_NOT_USED_M
 * @brief Removes a warning from a not used var when in debug mode
 *
 * When not in debug mode, generates a message: "Warning: SACI_NOT_USED_M is used —
 * remove before release!"
 * @param[in] x The variable to have it's not used warning suppresed
 */
#ifdef SACI_DEBUG_MODE

#  define SACI_NOT_USED_M(x) (void)(x)
#else
#  define SACI_NOT_USED_M(x) \
      _Pragma("message(\"Warning: SACI_NOT_USED_M is used — remove before release!\")")(void)(x)

#endif // SACI_DEBUG_MODE

#ifdef SACI_DEBUG_MODE
#  define SACI_TODO_M \
      printf("Not implemented %s:%d\n", __FILE__, __LINE__)
#else
#  define SACI_TODO_M                                        \
      printf("Not implemented %s:%d\n", __FILE__, __LINE__); \
      abort()
#endif // SACI_DEBUG_MODE

/**
 * @define SACI_MIN_M
 * @brief Returns the smallest of two values
 *
 * @param[in] x The smallest value
 */
#define SACI_MIN_M(x, y) ((x) < (y) ? (x) : (y))

/**
 * @define SACI_MAX_M
 * @brief Returns the biggest of two values
 *
 * @param[in] x The biggest value
 */
#define SACI_MAX_M(x, y) ((x) > (y) ? (x) : (y))

#ifndef SACI_MAX_TEXTURES
#  define SACI_MAX_TEXTURES 16
#endif // SACI_MAX_TEXTURES

#define SACI_TEXTURE_INVALID (UINT32_MAX)

#define SACI_HAS_FLAG_M(var, flag) (((var) & (flag)) == (flag))

#define SACI_ADD_FLAG_M(var, flag) ((var) |= (flag))

#define SACI_REMOVE_FLAG_M(var, flag) ((var) &= ~(flag))

#define SACI_TOGGLE_FLAG_M(var, flag) ((var) ^= (flag))

// 2^20
#define SACI_MAX_STRING_SIZE 1048576

// For some reason, strlen does not count the '\0' char. Why???
#define SACI_STRSIZE_M(str) ((strnlen(str, SACI_MAX_STRING_SIZE) + 1) * sizeof(char))

// 2̛̛^14
#define SACI_MAX_CONFIG_FIELD_STRING_SIZE 16384

#endif // SACI_UTILS_SU_GENERAL_H
