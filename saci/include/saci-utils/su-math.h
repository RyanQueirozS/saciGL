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
#define sa_Deg_To_Rad_m(x) ((x) * sa_DEG2RAD_MULT)
#define sa_RAD2DEG_MULT (180.0f / SACI_PI)
#define sa_Rad_To_Deg_m(x) ((x) * sa_RAD2DEG_MULT)

/* === Vec3 Operations === */

sa_vec3_t sa_Vec3_Subtract(sa_vec3_t a, sa_vec3_t b);
sa_vec3_t sa_Vec3_Add(sa_vec3_t a, sa_vec3_t b);
sa_vec3_t sa_Vec3_Scale(sa_vec3_t v, float scalar);
sa_vec3_t sa_Vec3_Normalize(sa_vec3_t v);
sa_vec3_t sa_Vec3_Cross(sa_vec3_t a, sa_vec3_t b);
float sa_Vec3_Dot(sa_vec3_t a, sa_vec3_t b);

/* === Color Operations === */

sa_color_t sa_Color_From_Hex(sa_u32_t hex);
sa_color_t sa_Color_From_U8(sa_u8_t r, sa_u8_t g, sa_u8_t b, sa_u8_t a);
sa_u32_t sa_Color_To_Hex(sa_color_t color);

/* === Mat Operations === */

sa_mat4_t sa_Mat4_Multiply(sa_mat4_t a, sa_mat4_t b);
sa_mat4_t sa_Mat4_Identity(void);
sa_mat4_t sa_Mat4_Look_At(sa_vec3_t eye, sa_vec3_t center, sa_vec3_t up);
sa_mat4_t sa_Mat4_Perspective(float fov, float aspect, float near, float far);
sa_mat4_t sa_Mat4_Ortho(float left, float right, float bottom, float top, float near, float far);
sa_mat4_t sa_Mat4_Rotate_X(sa_mat4_t mat, float angle);
sa_mat4_t sa_Mat4_Rotate_Y(sa_mat4_t mat, float angle);
sa_mat4_t sa_Mat4_Rotate_Z(sa_mat4_t mat, float angle);
sa_mat4_t sa_Mat4_Scale(float sx, float sy, float sz);
sa_mat4_t sa_Mat4_Translate(float tx, float ty, float tz);
sa_mat4_t sa_Mat4_Model_Matrix(sa_vec3_t position, sa_vec3_t rotation, sa_vec3_t scale);

#endif
