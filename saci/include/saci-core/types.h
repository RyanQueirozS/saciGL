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
typedef std::uintptr_t su_UintPtr;
#else
typedef uintptr_t SaciUintPtr;
#endif

/* === Boolean Constants === */

#define SACI_TRUE true

#define SACI_FALSE false

typedef bool SaciBool;

/* === Identifiers === */

typedef SaciU32 SaciShaderId;

typedef SaciU32 SaciTextureId;

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
    SACI_TYPE_U8 = 1,  // unsigned int8
    SACI_TYPE_U16 = 2, // unsigned int16
    SACI_TYPE_U32 = 3, // unsigned int32
    SACI_TYPE_U64 = 4, // unsigned int64
    SACI_TYPE_S8 = 5,  // signed int8
    SACI_TYPE_S16 = 6, // signed int16
    SACI_TYPE_S32 = 7, // signed int32
    SACI_TYPE_S64 = 8, // signed int64

    SACI_TYPE_BOOL = 9, // boolean

    SACI_TYPE_SHADERID = 10,  // unsgined int32
    SACI_TYPE_TEXTUREID = 11, // unsgined int32
    SACI_TYPE_BUFFERID = 12,  // unsigned int32

    SACI_TYPE_UV = 13,   // 2 floats (u, v)
    SACI_TYPE_VEC2 = 14, // 2 floats (x, y)
    SACI_TYPE_VEC3 = 15, // 3 floats (x, y, z)
    SACI_TYPE_VEC4 = 16, // 4 floats (x, y, z, w)

    SACI_TYPE_COLOR = 17, // 4 floats (r, g, b, a)

    SACI_TYPE_MAT2 = 18, // 2x2 matrix
    SACI_TYPE_MAT3 = 19, // 3x3 matrix
    SACI_TYPE_MAT4 = 20, // 4x4 matrix

    SACI_TYPE_MAT2X3 = 21, // 2 columns, 3 rows
    SACI_TYPE_MAT2X4 = 22, // 2 columns, 4 rows
    SACI_TYPE_MAT3X2 = 23, // 3 columns, 2 rows
    SACI_TYPE_MAT3X4 = 24, // 3 columns, 4 rows
    SACI_TYPE_MAT4X2 = 25, // 4 columns, 2 rows
    SACI_TYPE_MAT4X3 = 26, // 4 columns, 3 rows

    SACI_TYPE_SAMPLER2D = 27,
} SaciDataType;

#define SACI_TYPE_MAX 26
static const SaciU64 SACI_G_TYPE_SIZE_TABLE[] = {
    0, // index 0 (unused)

    1, // su_TYPE_U8        = 1
    2, // su_TYPE_U16       = 2
    4, // su_TYPE_U32       = 3
    8, // su_TYPE_U64       = 4
    1, // su_TYPE_S8        = 5
    2, // su_TYPE_S16       = 6
    4, // su_TYPE_S32       = 7
    8, // su_TYPE_S64       = 8

    1, // su_TYPE_BOOL      = 9

    4, // su_TYPE_SHADERID  = 10
    4, // su_TYPE_TEXTUREID = 11
    4, // su_TYPE_BUFFERID  = 12

    8,  // su_TYPE_UV        = 13 (2 floats)
    8,  // su_TYPE_VEC2      = 14 (2 floats)
    12, // su_TYPE_VEC3      = 15 (3 floats)
    16, // su_TYPE_VEC4      = 16 (4 floats)

    16, // su_TYPE_COLOR     = 17 (4 floats, e.g. RGBA)

    16, // su_TYPE_MAT2      = 18 (2x2 floats = 4 floats)
    36, // su_TYPE_MAT3      = 19 (3x3 = 9 floats)
    64, // su_TYPE_MAT4      = 20 (4x4 = 16 floats)

    24, // su_TYPE_MAT2X3    = 21 (2 columns * 3 rows = 6 floats)
    32, // su_TYPE_MAT2X4    = 22 (2 * 4 = 8 floats)
    24, // su_TYPE_MAT3X2    = 23 (3 * 2 = 6 floats)
    48, // su_TYPE_MAT3X4    = 24 (3 * 4 = 12 floats)
    32, // su_TYPE_MAT4X2    = 25 (4 * 2 = 8 floats)
    48  // su_TYPE_MAT4X3    = 26 (4 * 3 = 12 floats)
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
