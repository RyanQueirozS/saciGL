/**
 * @file types.h
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
#include <inttypes.h>

/* === Typedefs === */

typedef uint64_t SaciU64;

typedef uint32_t SaciU32;

typedef uint16_t SaciU16;

typedef uint8_t SaciU8;

typedef int64_t SaciS64;

typedef int32_t SaciS32;

typedef int16_t SaciS16;

typedef int8_t SaciS8;

#define SACI_FMTU8 "%" PRIu8
#define SACI_FMTU16 "%" PRIu16
#define SACI_FMTU32 "%" PRIu32
#define SACI_FMTU64 "%" PRIu64

#define SACI_FMTS8 "%" PRId8
#define SACI_FMTS16 "%" PRId16
#define SACI_FMTS32 "%" PRId32
#define SACI_FMTS64 "%" PRId64

#if defined(__cplusplus) && __cplusplus < 201103L
#  include <cstddef>
typedef std::uintptr_t SaciUintPtr;
#else
typedef uintptr_t SaciUintPtr;
#endif

/* === Boolean Constants === */

typedef bool SaciBool;

#define SACI_TRUE (SaciBool)1
#define SACI_FALSE (SaciBool)0

/* === Identifiers === */

typedef SaciU32 SaciShaderId;

typedef SaciU32 SaciBufferId;

/* === Vector Structures === */

typedef struct {
    float u; /**< U compoonent */
    float v; /**< V compoonent */
} SaciUv;

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
} SaciVec2;

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
} SaciVec3;

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
    float w; /**< W compoonent */
} SaciVec4;

typedef struct {
    float r; /**< Red compoonent */
    float g; /**< Green compoonent */
    float b; /**< Blue compoonent */
    float a; /**< Alpha (transparency) compoonent */
} SaciColor;

typedef struct {
    float data[4][4]; /**< 4x4 matrix of floats */
} SaciMat4;

/* === Types === */

typedef enum {
    SACI_TYPE_UNKOWN = 0,
    SACI_TYPE_VOIDPTR = 1,
    SACI_TYPE_U8 = 2,  // unsigned int8
    SACI_TYPE_U16 = 3, // unsigned int16
    SACI_TYPE_U32 = 4, // unsigned int33
    SACI_TYPE_U64 = 5, // unsigned int64
    SACI_TYPE_S8 = 6,  // signed int8
    SACI_TYPE_S16 = 7, // signed int16
    SACI_TYPE_S32 = 8, // signed int32
    SACI_TYPE_S64 = 9, // signed int64

    SACI_TYPE_CHAR = 10,
    SACI_TYPE_STRING = 11,

    SACI_TYPE_FLOAT = 12,
    SACI_TYPE_DOUBLE = 13,

    SACI_TYPE_BOOL = 14, // boolean

    SACI_TYPE_SHADERID = 15,  // unsgined int32
    SACI_TYPE_TEXTUREID = 16, // unsgined int32
    SACI_TYPE_BUFFERID = 17,  // unsigned int34

    SACI_TYPE_UV = 18,   // 2 floats (u, v)
    SACI_TYPE_VEC2 = 19, // 2 floats (x, y)
    SACI_TYPE_VEC3 = 20, // 3 floats (x, y, z)
    SACI_TYPE_VEC4 = 21, // 4 floats (x, y, z, w)

    SACI_TYPE_COLOR = 22, // 4 floats (r, g, b, a)

    SACI_TYPE_MAT2 = 23, // 2x2 matrix
    SACI_TYPE_MAT3 = 24, // 3x3 matrix
    SACI_TYPE_MAT4 = 25, // 4x4 matrix

    SACI_TYPE_MAT2X3 = 26, // 2 columns, 3 rows
    SACI_TYPE_MAT2X4 = 27, // 2 columns, 4 rows
    SACI_TYPE_MAT3X2 = 28, // 3 columns, 2 rows
    SACI_TYPE_MAT3X4 = 29, // 3 columns, 4 rows
    SACI_TYPE_MAT4X2 = 30, // 4 columns, 2 rows
    SACI_TYPE_MAT4X3 = 31, // 4 columns, 3 rows

    SACI_TYPE_NULL = 32,

    SACI_TYPE_SAMPLER2D = 33,
} SaciDataType;

#define SACI_TYPE_MAX SACI_TYPE_SAMPLER2D
static const SaciU64 SACI_G_TYPE_SIZE_TABLE[SACI_TYPE_MAX + 1] = {
    0, // UNKOWN
    0, // VOIDPTR

    1, // SACI_TYPE_U8         = 1
    2, // SACI_TYPE_U16        = 2
    4, // SACI_TYPE_U32        = 3
    8, // SACI_TYPE_U64        = 4
    1, // SACI_TYPE_S8         = 5
    2, // SACI_TYPE_S16        = 6
    4, // SACI_TYPE_S32        = 7
    8, // SACI_TYPE_S64        = 8

    4, // SACI_TYPE_FLOAT      = 9
    8, // SACI_TYPE_DOUBLE     = 10

    1, // SACI_TYPE_CHAR       = 11
    0, // SACI_TYPE_STRING     = 12

    1, // SACI_TYPE_BOOL       = 13

    4, // SACI_TYPE_SHADERID   = 14
    4, // SACI_TYPE_TEXTUREID  = 15
    4, // SACI_TYPE_BUFFERID   = 16

    8,  // SACI_TYPE_UV        = 17 (2 floats)
    8,  // SACI_TYPE_VEC2      = 18 (2 floats)
    12, // SACI_TYPE_VEC3      = 19 (3 floats)
    16, // SACI_TYPE_VEC4      = 20 (4 floats)

    16, // SACI_TYPE_COLOR     = 21 (4 floats, e.g. RGBA)

    16, // SACI_TYPE_MAT2      = 22 (2x2 floats = 4 floats)
    36, // SACI_TYPE_MAT3      = 23 (3x3 = 9 floats)
    64, // SACI_TYPE_MAT4      = 24 (4x4 = 16 floats)

    24, // SACI_TYPE_MAT2X3    = 25 (2 columns * 3 rows = 6 floats)
    32, // SACI_TYPE_MAT2X4    = 26 (2 * 4 = 8 floats)
    24, // SACI_TYPE_MAT3X2    = 27 (3 * 2 = 6 floats)
    48, // SACI_TYPE_MAT3X4    = 28 (3 * 4 = 12 floats)
    32, // SACI_TYPE_MAT4X2    = 29 (4 * 2 = 8 floats)
    48, // SACI_TYPE_MAT4X3    = 30 (4 * 3 = 12 floats)

    0, // SACI_TYPE_NULL
};

// TODO move to memory/
typedef enum {
    SACI_REALLOCATION_KIND_REALLOC = 0,
    SACI_REALLOCATION_KIND_MALLOC_FREE = 1,
    SACI_REALLOCATION_KIND_FIXED_SIZE = 2,
} SaciReallocationKind;

/* === Rendering Specific === */

// TODO move
typedef void* (*SaciGLProc)(const char* name);
typedef void* SaciWindowPosHandler;
typedef void* SaciWindowSizeHandler;
typedef void* SaciEventMousePosHandler;

#endif // SACI_UTILS_SU_TYPES_H
