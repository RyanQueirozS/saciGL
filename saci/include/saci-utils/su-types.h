/**
 * @file su-types.h
 * @brief This header defines common types and structures used in the saciCORE and saciLIB.
 *
 * @details
 * This file provides typedefs for both signed and unsigned integer types, boolean constants,
 * as well as common vector, color, and matrix structures.
 * It aims to simplify the use of types across the saciCORE and saciLIB and ensure consistency.
 */

#ifndef __SACI_UTILS_SU_TYPES_H__
#define __SACI_UTILS_SU_TYPES_H__

#include <stdbool.h>
#include <stdint.h>

/* === Typedefs === */

/**
 * @typedef saci_u64
 * @brief Unsigned 64-bit integer.
 */
typedef uint64_t saci_u64;

/**
 * @typedef saci_u32
 * @brief Unsigned 32-bit integer.
 */
typedef uint32_t saci_u32;

/**
 * @typedef saci_u16
 * @brief Unsigned 16-bit integer.
 */
typedef uint16_t saci_u16;

/**
 * @typedef saci_u8
 * @brief Unsigned 8-bit integer.
 */
typedef uint8_t saci_u8;

/**
 * @typedef saci_s64
 * @brief Signed 64-bit integer.
 */
typedef int64_t saci_s64;

/**
 * @typedef saci_s32
 * @brief Signed 32-bit integer.
 */
typedef int32_t saci_s32;

/**
 * @typedef saci_s16
 * @brief Signed 16-bit integer.
 */
typedef int16_t saci_s16;

/**
 * @typedef saci_s8
 * @brief Signed 8-bit integer.
 */
typedef int8_t saci_s8;

/* === Boolean Constants === */

/**
 * @def SACI_TRUE
 * @brief boolean true constant
 */
#define SACI_TRUE true

/**
 * @def SACI_FALSE
 * @brief boolean false constant
 */
#define SACI_FALSE false

/**
 * @typedef saci_Bool
 * @brief Boolean type for representing true/false.
 */
typedef bool saci_Bool;

/* === Identifiers === */

/**
 * @typedef saci_ShaderID
 * @brief Represents an identifier for shaders (32-bit unsigned integer).
 */
typedef saci_u32 saci_ShaderID;

/**
 * @typedef saci_TextureID
 * @brief Represents an identifier for textures (32-bit unsigned integer).
 */
typedef saci_u32 saci_TextureID;

/* === Vector Structures === */

/**
 * @struct saci_Vec2
 * @brief Represents a 2D vector with X and Y components.
 */
typedef struct saci_Vec2 {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
} saci_Vec2;

/**
 * @struct saci_Vec3
 * @brief Represents a 3D vector with X, Y, and Z components.
 */
typedef struct saci_Vec3 {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
} saci_Vec3;

/**
 * @struct saci_Vec4
 * @brief Represents a 4D vector with X, Y, Z, and W components.
 */
typedef struct saci_Vec4 {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
    float w; /**< W compoonent */
} saci_Vec4;

/* === Color Structure === */

/**
 * @struct saci_Color
 * @brief Represents a color with red, green, blue, and alpha components (RGBA).
 * @note values should be stored from 0 to 1.
 */
typedef struct saci_Color {
    float r; /**< Red compoonent */
    float g; /**< Green compoonent */
    float b; /**< Blue compoonent */
    float a; /**< Alpha (transparency) compoonent */
} saci_Color;

/* === Matrix Structure === */

/**
 * @struct saci_Mat4
 * @brief Represents a 4x4 matrix used for transformations.
 */
typedef struct saci_Mat4 {
    float m[4][4]; /**< 4x4 matrix of floats */
} saci_Mat4;

#endif
