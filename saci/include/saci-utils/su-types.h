/**
 * @file su-types.h
 * @brief This header defines common types and structures used in the saciCORE and
 * saciLIB.
 *
 * @details
 * This file provides typedefs for both signed and unsigned integer types, boolean
 * constants, as well as common vector, color, and matrix structures. It aims to simplify
 * the use of types across the saciCORE and saciLIB and ensure consistency.
 */

#ifndef __SACI_UTILS_SU_TYPES_H__
#define __SACI_UTILS_SU_TYPES_H__

#include <stdbool.h>
#include <stdint.h>

/* === Typedefs === */

typedef uint64_t sa_U64_t;

typedef uint32_t sa_U32_t;

typedef uint16_t sa_U16_t;

typedef uint8_t  sa_U8_t;

typedef int64_t  sa_S64_t;

typedef int32_t  sa_S32_t;

typedef int16_t  sa_S16_t;

typedef int8_t   sa_S8_t;

/* === Boolean Constants === */

#define sa_TRUE true

#define sa_FALSE false

typedef bool sa_Bool_t;

/* === Identifiers === */

typedef sa_U32_t sa_Shader_ID;

typedef sa_U32_t sa_Texture_ID;

/* === Vector Structures === */

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
} sa_Vec2_t;

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
} sa_Vec3_t;

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
    float w; /**< W compoonent */
} sa_Vec4_t;

/* === Color Structure === */

typedef struct sa_Color_t {
    float r; /**< Red compoonent */
    float g; /**< Green compoonent */
    float b; /**< Blue compoonent */
    float a; /**< Alpha (transparency) compoonent */
} sa_Color_t;

/* === Matrix Structure === */

typedef struct sa_Mat4_t {
    float m[4][4]; /**< 4x4 matrix of floats */
} sa_Mat4_t;

#endif
