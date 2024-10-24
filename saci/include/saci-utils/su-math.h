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

/**
 * @def SACI_PI
 * @brief Constant for PI (3.141592653589793).
 */
#define SACI_PI 3.141592653589793f

/**
 * @def SACI_DEG2RAD_M
 * @brief Constant for converting degrees to radians.
 */
#define SACI_DEG2RAD_M (SACI_PI / 180.0f)

/**
 * @def SACI_DEG2RAD
 * @brief Macro to convert degrees to radians.
 *
 * @param x Angle in degrees.
 * @return The angle in radians
 */
#define SACI_DEG2RAD(x) ((x)*SACI_DEG2RAD_M)

/**
 * @def SACI_RAD2DEG_M
 * @brief Constant for converting radians to degrees.
 */
#define SACI_RAD2DEG_M (180.0f / SACI_PI)

/**
 * @def SACI_RAD2DEG
 * @brief Macro to convert radians to degrees.
 *
 * @param x Angle in radians.
 * @return The angle in degrees.
 */
#define SACI_RAD2DEG(x) ((x)*SACI_RAD2DEG_M)

/* === Initialization === */

/**
 * @brief Initializes the mathematical functions and constants.
 * @note Needed to use all of saciCORE and saciLIB.
 */
void saci_InitMath();

/* === Vec3 Operations === */

/**
 * @brief Subtracts two 3D vectors.
 *
 * @param a The first vector.
 * @param b The second vector.
 * @return The resulting vector after subtraction (a - b).
 */
saci_Vec3 saci_SubtractVec3(saci_Vec3 a, saci_Vec3 b);

/**
 * @brief Adds two 3D vectors.
 *
 * @param a The first vector.
 * @param b The second vector.
 * @return The resulting vector after addition (a + b).
 */
saci_Vec3 saci_AddVec3(saci_Vec3 a, saci_Vec3 b);

/**
 * @brief Multiplies a 3D vector.
 *
 * @param v The vector.
 * @param scalar The second vector.
 * @return The resulting vector after multiplication.
 */
saci_Vec3 saci_MultiplyVec3(saci_Vec3 v, float scalar);

/**
 * @brief Nomalizes a 3D vector.
 *
 * @param v The vector.
 * @return The normalized vector.
 */
saci_Vec3 saci_NormalizeVec3(saci_Vec3 v);

/**
 * @brief Computes the cross product of two 3D vectors.
 *
 * @param a The vector.
 * @param b The vector.
 * @return The resulting vector that is orthogonal to both a and b.
 */
saci_Vec3 saci_CrossVec3(saci_Vec3 a, saci_Vec3 b);

/**
 * @brief Computes the dot product of two 3D vectors.
 *
 * @param a The first vector.
 * @param b The second vector.
 * @return The dot product of a and b.
 */
float saci_DotVec3(saci_Vec3 a, saci_Vec3 b);

/* === Color Operations === */

/**
 * @brief Creates a color from a hexadecimal value.
 *
 * @param hex The hexadecimal color value.
 * @return The resulting saci_Color.
 */
saci_Color saci_ColorFromHex(saci_u32 hex);

/**
 * @brief Creates a color from RGBA values.
 *
 * @param r The red component (0-255).
 * @param g The green component (0-255).
 * @param b The blue component (0-255).
 * @param a The alpha component (0-255).
 * @return The resulting saci_Color.
 */
saci_Color saci_ColorFromU8(saci_u8 r, saci_u8 g, saci_u8 b, saci_u8 a);

/**
 * @brief Converts a saci_Color to a hexadecimal value.
 *
 * @param color The saci_Color to convert.
 * @return The hexadecimal representation of the color.
 */

saci_u32 saci_HexFromColor(saci_Color color);

/* === Mat4 Operations === */

/**
 * @brief Multiplies two 4x4 matrices.
 *
 * @param a The first matrix.
 * @param b The second matrix.
 * @return The resulting matrix after multiplication (a * b).
 */
saci_Mat4 saci_MultiplyMat4(saci_Mat4 a, saci_Mat4 b);

/**
 * @brief Creates an identity matrix.
 *
 * @return The identity saci_Mat4 matrix.
 */
saci_Mat4 saci_IdentityMat4();

/**
 * @brief Creates a view matrix for a camera using the LookAt method.
 *
 * @param eye The position of the camera.
 * @param center The point the camera is looking at.
 * @param up The up direction.
 * @return The resulting view matrix.
 */
saci_Mat4 saci_LookAtMat4(saci_Vec3 eye, saci_Vec3 center, saci_Vec3 up);

/**
 * @brief Creates a perspective projection matrix.
 *
 * @param fov The field of view in degrees.
 * @param aspect The aspect ratio.
 * @param near The near clipping plane distance.
 * @param far The far clipping plane distance.
 * @return The resulting perspective matrix.
 */
saci_Mat4 saci_PerspectiveMat4(float fov, float aspect, float near, float far);

/**
 * @brief Creates an orthographic projection matrix.
 *
 * @param left The left plane.
 * @param right The right plane.
 * @param bottom The bottom plane.
 * @param top The top plane.
 * @param near The near clipping plane distance.
 * @param far The far clipping plane distance.
 * @return The resulting orthographic matrix.
 */
saci_Mat4 saci_OrthoMat4(float left, float right, float bottom, float top, float near, float far);

/**
 * @brief Rotates a matrix around the X-axis.
 *
 * @param mat The matrix to rotate.
 * @param angle The angle of rotation in radians.
 * @return The resulting rotated matrix.
 */
saci_Mat4 saci_RotateMat4_X(saci_Mat4 mat, float angle);

/**
 * @brief Rotates a matrix around the Y-axis.
 *
 * @param mat The matrix to rotate.
 * @param angle The angle of rotation in radians.
 * @return The resulting rotated matrix.
 */
saci_Mat4 saci_RotateMat4_Y(saci_Mat4 mat, float angle);

#endif
