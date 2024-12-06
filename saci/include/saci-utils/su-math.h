/**
 * @file su-math.h
 * @brief This header defines common functions and constants used in the saciCORE and saciLIB.
 *
 * @details
 * This file provides mathematical constants, utility functions for common vector operations, color
 * manipulation, and matrix manipulations.
 * It can and should be used outside of saci as it's supposed to be modular and performat for most
 * applications
 */

#ifndef __SACI_UTILS_SU_MATH_H__
#define __SACI_UTILS_SU_MATH_H__

#include "saci-utils/su-types.h"

/* === Constants === */

#define sa_PI 3.141592653589793f

#define sa_DEG2RAD_MULT (sa_PI / 180.0f)

#define sa_DEG2RAD_m(x) ((x)*sa_DEG2RAD_MULT)

#define sa_RAD2DEG_MULT (180.0f / SACI_PI)

#define sa_RAD2DEG_m(x) ((x)*sa_RAD2DEG_MULT)

/* === Initialization === */

void sa_Math_Init();

/* === Vec3 Operations === */

saci_Vec3 sa_Vec3_Subtract(saci_Vec3 a, saci_Vec3 b);

saci_Vec3 sa_Vec3_Add(saci_Vec3 a, saci_Vec3 b);

saci_Vec3 sa_Vec3_Scale(saci_Vec3 v, float scalar);

saci_Vec3 sa_Vec3_Normalize(saci_Vec3 v);

saci_Vec3 sa_Vec3_Cross(saci_Vec3 a, saci_Vec3 b);

float     sa_Vec3_Dot(saci_Vec3 a, saci_Vec3 b);

/* === Color Operations === */

saci_Color sa_Color_From_Hex(saci_u32 hex);

saci_Color sa_Color_From_U8(saci_u8 r, saci_u8 g, saci_u8 b, saci_u8 a);

saci_u32   sa_Color_To_Hex(saci_Color color);

/* === Mat Operations === */

saci_Mat4 sa_Mat4_Multiply(saci_Mat4 a, saci_Mat4 b);

saci_Mat4 sa_Mat4_Identity();

saci_Mat4 sa_Mat4_Look_At(saci_Vec3 eye, saci_Vec3 center, saci_Vec3 up);

saci_Mat4 sa_Mat4_Perspective(float fov, float aspect, float near, float far);

saci_Mat4 sa_Mat4_Ortho(float left, float right, float bottom, float top, float near, float far);

saci_Mat4 sa_Mat4_Rotate_X(saci_Mat4 mat, float angle);

saci_Mat4 sa_Mat_Rotate_Y(saci_Mat4 mat, float angle);

saci_Mat4 acMat4_Scale(float sx, float sy, float sz);

saci_Mat4 sa_Mat4_Translate(float tx, float ty, float tz);

saci_Mat4 sa_Mat4_Model_Matrix(saci_Vec3 position, saci_Vec3 rotation, saci_Vec3 scale);

#endif
