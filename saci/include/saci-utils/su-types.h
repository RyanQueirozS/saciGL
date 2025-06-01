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

#ifndef SACI_UTILS_SU_TYPES_H
#define SACI_UTILS_SU_TYPES_H

#include <stdbool.h>
#include <stdint.h>

/* === Typedefs === */

typedef uint64_t sa_u64;

typedef uint32_t sa_u32;

typedef uint16_t sa_u16;

typedef uint8_t sa_u8;

typedef int64_t sa_s64;

typedef int32_t sa_s32;

typedef int16_t sa_s16;

typedef int8_t sa_s8;

/* === Boolean Constants === */

#define sa_TRUE true

#define sa_FALSE false

typedef bool sa_bool;

/* === Identifiers === */

typedef sa_u32 sa_shaderId;

typedef sa_u32 sa_textureId;

typedef sa_u32 sa_bufferId;

/* === Vector Structures === */

typedef struct {
    float u; /**< U compoonent */
    float v; /**< V compoonent */
} sa_uv;

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
} sa_vec2;

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
} sa_vec3;

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
    float w; /**< W compoonent */
} sa_vec4;

/* === Color Structure === */

typedef struct {
    float r; /**< Red compoonent */
    float g; /**< Green compoonent */
    float b; /**< Blue compoonent */
    float a; /**< Alpha (transparency) compoonent */
} sa_color;

/* === Matrix Structure === */

typedef struct {
    float data[4][4]; /**< 4x4 matrix of floats */
} sa_mat4;

/* === Types === */

typedef enum {
    SA_TYPE_U8 = 1,  // unsigned int8
    SA_TYPE_U16 = 2, // unsigned int16
    SA_TYPE_U32 = 3, // unsigned int32
    SA_TYPE_U64 = 4, // unsigned int64
    SA_TYPE_S8 = 5,  // signed int8
    SA_TYPE_S16 = 6, // signed int16
    SA_TYPE_S32 = 7, // signed int32
    SA_TYPE_S64 = 8, // signed int64

    SA_TYPE_BOOL = 9, // boolean

    SA_TYPE_SHADERID = 10,  // unsgined int32
    SA_TYPE_TEXTUREID = 11, // unsgined int32
    SA_TYPE_BUFFERID = 12,  // unsigned int32

    SA_TYPE_UV = 13,   // 2 floats (u, v)
    SA_TYPE_VEC2 = 14, // 2 floats (x, y)
    SA_TYPE_VEC3 = 15, // 3 floats (x, y, z)
    SA_TYPE_VEC4 = 16, // 4 floats (x, y, z, w)

    SA_TYPE_COLOR = 17, // 4 floats (r, g, b, a)

    SA_TYPE_MAT2 = 18, // 2x2 matrix
    SA_TYPE_MAT3 = 19, // 3x3 matrix
    SA_TYPE_MAT4 = 20, // 4x4 matrix

    SA_TYPE_MAT2X3 = 21, // 2 columns, 3 rows
    SA_TYPE_MAT2X4 = 22, // 2 columns, 4 rows
    SA_TYPE_MAT3X2 = 23, // 3 columns, 2 rows
    SA_TYPE_MAT3X4 = 24, // 3 columns, 4 rows
    SA_TYPE_MAT4X2 = 25, // 4 columns, 2 rows
    SA_TYPE_MAT4X3 = 26, // 4 columns, 3 rows
} sa_dataType;

#define SA_TYPE_MAX 26
static const sa_u64 SA_SIZE_OF_TYPE[] = {
    0, // index 0 (unused)

    1, // SA_TYPE_U8        = 1
    2, // SA_TYPE_U16       = 2
    4, // SA_TYPE_U32       = 3
    8, // SA_TYPE_U64       = 4
    1, // SA_TYPE_S8        = 5
    2, // SA_TYPE_S16       = 6
    4, // SA_TYPE_S32       = 7
    8, // SA_TYPE_S64       = 8

    1, // SA_TYPE_BOOL      = 9

    4, // SA_TYPE_SHADERID  = 10
    4, // SA_TYPE_TEXTUREID = 11
    4, // SA_TYPE_BUFFERID  = 12

    8,  // SA_TYPE_UV        = 13 (2 floats)
    8,  // SA_TYPE_VEC2      = 14 (2 floats)
    12, // SA_TYPE_VEC3      = 15 (3 floats)
    16, // SA_TYPE_VEC4      = 16 (4 floats)

    16, // SA_TYPE_COLOR     = 17 (4 floats, e.g. RGBA)

    16, // SA_TYPE_MAT2      = 18 (2x2 floats = 4 floats)
    36, // SA_TYPE_MAT3      = 19 (3x3 = 9 floats)
    64, // SA_TYPE_MAT4      = 20 (4x4 = 16 floats)

    24, // SA_TYPE_MAT2X3    = 21 (2 columns * 3 rows = 6 floats)
    32, // SA_TYPE_MAT2X4    = 22 (2 * 4 = 8 floats)
    24, // SA_TYPE_MAT3X2    = 23 (3 * 2 = 6 floats)
    48, // SA_TYPE_MAT3X4    = 24 (3 * 4 = 12 floats)
    32, // SA_TYPE_MAT4X2    = 25 (4 * 2 = 8 floats)
    48  // SA_TYPE_MAT4X3    = 26 (4 * 3 = 12 floats)
};

#endif // SACI_UTILS_SU_TYPES_H
