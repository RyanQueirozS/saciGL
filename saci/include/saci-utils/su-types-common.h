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

typedef uint64_t su_U64;

typedef uint32_t su_U32;

typedef uint16_t su_U16;

typedef uint8_t su_U8;

typedef int64_t su_S64;

typedef int32_t su_S32;

typedef int16_t su_S16;

typedef int8_t su_S8;

/* === Boolean Constants === */

#define su_TRUE true

#define su_FALSE false

typedef bool su_Bool;

/* === Identifiers === */

typedef su_U32 su_ShaderId;

typedef su_U32 su_TextureId;

typedef su_U32 su_BufferId;

/* === Vector Structures === */

typedef struct {
    float u; /**< U compoonent */
    float v; /**< V compoonent */
} su_Uv;

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
} su_Vec2;

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
    float w; /**< W compoonent */
} su_Vec4;

/* === Types === */

typedef enum {
    su_TYPE_U8 = 1,  // unsigned int8
    su_TYPE_U16 = 2, // unsigned int16
    su_TYPE_U32 = 3, // unsigned int32
    su_TYPE_U64 = 4, // unsigned int64
    su_TYPE_S8 = 5,  // signed int8
    su_TYPE_S16 = 6, // signed int16
    su_TYPE_S32 = 7, // signed int32
    su_TYPE_S64 = 8, // signed int64

    su_TYPE_BOOL = 9, // boolean

    su_TYPE_SHADERID = 10,  // unsgined int32
    su_TYPE_TEXTUREID = 11, // unsgined int32
    su_TYPE_BUFFERID = 12,  // unsigned int32

    su_TYPE_UV = 13,   // 2 floats (u, v)
    su_TYPE_VEC2 = 14, // 2 floats (x, y)
    su_TYPE_VEC3 = 15, // 3 floats (x, y, z)
    su_TYPE_VEC4 = 16, // 4 floats (x, y, z, w)

    su_TYPE_COLOR = 17, // 4 floats (r, g, b, a)

    su_TYPE_MAT2 = 18, // 2x2 matrix
    su_TYPE_MAT3 = 19, // 3x3 matrix
    su_TYPE_MAT4 = 20, // 4x4 matrix

    su_TYPE_MAT2X3 = 21, // 2 columns, 3 rows
    su_TYPE_MAT2X4 = 22, // 2 columns, 4 rows
    su_TYPE_MAT3X2 = 23, // 3 columns, 2 rows
    su_TYPE_MAT3X4 = 24, // 3 columns, 4 rows
    su_TYPE_MAT4X2 = 25, // 4 columns, 2 rows
    su_TYPE_MAT4X3 = 26, // 4 columns, 3 rows

    su_TYPE_SAMPLER2D = 27,
} su_DataType;

#define su_TYPE_MAX 26
static const su_U64 su_SIZE_OF_TYPE[] = {
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
typedef enum su_ReallocationKind {
    su_REALLOCATION_KIND_REALLOC = 0,
    su_REALLOCATION_KIND_MALLOC_FREE = 1,
    su_REALLOCATION_KIND_FIXED_SIZE = 2,
} su_ReallocationKind;

/* === Rendering Specific === */

// TODO move to types/su-types-backend.h
typedef void* (*su_GLProc)(const char* name);
typedef void* su_Window;
typedef void* su_Monitor;
typedef void (*su_WindowPosHandler)(su_Window* window, int pos_x, int pos_y);
typedef void (*su_WindowSizeHandler)(su_Window* window, int width, int height);
typedef void (*su_EventMousePosHandler)(su_Window window, double posx, double posy);

#endif // SACI_UTILS_SU_TYPES_H
