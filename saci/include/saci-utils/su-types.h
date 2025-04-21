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

#endif
