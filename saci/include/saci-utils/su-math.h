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
 * @define sa_PI
 * @brief PI declaration up to 14 digits
 */
#define sa_PI 3.141592653589793f

/**
 * @define sa_DEG2RAD_MULT
 * @brief Multiplication constant value for degree to radian operations
 */
#define sa_DEG2RAD_MULT (sa_PI / 180.0f)

/**
 * @define sa_Deg_To_Rad_m
 * @brief Macro that returns a degree from a radian
 */
#define sa_Deg_To_Rad_m(x) ((x) * sa_DEG2RAD_MULT)

/**
 * @define sa_RAD2DEG_MULT
 * @brief Multiplication constant value for radian to degree operations
 */
#define sa_RAD2DEG_MULT (180.0f / SACI_PI)

/**
 * @define sa_Rad_To_Deg_m
 * @brief Macro that returns a radian from a degree
 */
#define sa_Rad_To_Deg_m(x) ((x) * sa_RAD2DEG_MULT)

/* === Vec3 Operations === */

/**
 * @fn sa_Vec3_Subtract
 * @brief Returns a new vector from a vector subtracted from another
 *
 * @param[in] a The base vector that will be subtracted
 * @param[in] b The vetor that will subtract
 *
 * @return sa_vec3 The result of the subtraction
 */
sa_vec3 sa_Vec3_Subtract(sa_vec3 a, sa_vec3 b);

/**
 * @fn sa_Vec3_Add
 * @brief Returns a new vector from a vector added from another
 *
 * @param[in] a The vector that will be added
 * @param[in] b The vetor that will added
 *
 * @return sa_vec3 The result of the subtraction
 */
sa_vec3 sa_Vec3_Add(sa_vec3 a, sa_vec3 b);

/**
 * @fn sa_Vec3_Scale
 * @brief Returns a new scaled vector
 *
 * @param[in] a The vector that will be added
 * @param[in] b The vetor that will added
 *
 * @return sa_vec3 The result of the scalation
 */
sa_vec3 sa_Vec3_Scale(sa_vec3 v, float scalar);

/**
 * @fn sa_Vec3_Normalize
 * @brief Returns a normalized vector to unit length (1)
 *
 * @param[in] v The vector to normalize
 *
 * @return sa_vec3 The normalized vector with a length of 1
 */
sa_vec3 sa_Vec3_Normalize(sa_vec3 v);

/**
 * @fn sa_Vec3_Cross
 * @brief Returns the computed cross product of two vectors
 *
 * @param[in] a The first vector
 * @param[in] b The second vector
 *
 * @return sa_vec3 The result of the cross product of vectors a and b
 */

sa_vec3 sa_Vec3_Cross(sa_vec3 a, sa_vec3 b);

/**
 * @fn sa_Vec3_Dot
 * @brief Returns the computed dot product of two vectors
 *
 * @param[in] a The first vector
 * @param[in] b The second vector
 *
 * @return float The result of the dot product of vectors a and b
 */
float sa_Vec3_Dot(sa_vec3 a, sa_vec3 b);

/* === Color Operations === */

/**
 * @fn sa_Color_From_Hex
 * @brief Returns a new sa_color from a given hex code
 *
 * @param[in] hex The hex code 0x12345678
 *
 * @return sa_color The new color based on the hex code
 */
sa_color sa_Color_From_Hex(sa_u32 hex);

/**
 * @fn sa_Color_From_U8
 * @brief Returns a new color from given rgba values
 *
 * @param[in] r The r component of the color
 * @param[in] g The g component of the color
 * @param[in] b The b component of the color
 * @param[in] a The a component of the color
 *
 * @return sa_color The new color based on the rgba value
 */
sa_color sa_Color_From_U8(sa_u8 r, sa_u8 g, sa_u8 b, sa_u8 a);

/**
 * @fn sa_Color_To_Hex
 * @brief Returns a new hex code from a color
 *
 * @param[in] color The color to be evaluated
 *
 * @return sa_u32 The new hex code
 */
sa_u32 sa_Color_To_Hex(sa_color color);

/* === Mat Operations === */

/**
 * @fn sa_Mat4_Multiply
 * @brief Returns a Multiplied matrix given another
 *
 * @param[in] a The matrix to be multiplied
 * @param[in] b The matrix to be multiplied
 *
 * @return sa_mat4 The multiplied matrix
 */

sa_mat4 sa_Mat4_Multiply(sa_mat4 a, sa_mat4 b);

/**
 * @fn sa_Mat4_Identity
 * @brief Returns an identiy matrix
 *
 * @return sa_mat4 The identity matrix
 */
sa_mat4 sa_Mat4_Identity(void);

/**
 * @fn sa_Mat4_Look_At
 * @brief Returns a "view" matrix
 *
 * @param[in] eye The camera's position in world space
 * @param[in] center The target point or the "look-at" point
 * @param[in] up The "up" direction for the camera
 *
 * @return sa_mat4 The new view matrix
 */
sa_mat4 sa_Mat4_Look_At(sa_vec3 eye, sa_vec3 center, sa_vec3 up);

/**
 * @fn sa_Mat4_Perspective
 * @brief Returns a perspective "projection" matrix
 *
 * @param[in] fov The camera's fov
 * @param[in] aspect The camera's aspect ratio
 * @param[in] near The camera's nearest renderable object
 * @param[in] far The camera's furthest renderable object
 *
 * @return sa_mat4 The new projection matrix
 */
sa_mat4 sa_Mat4_Perspective(float fov, float aspect, float near, float far);

/**
 * @fn sa_Mat4_Ortho
 * @brief Returns an orthogonal "projection" matrix
 *
 * @param[in] left The max coordinate of the left side
 * @param[in] right The max coordinate of the right side
 * @param[in] bottom The max coordinate of the bottom side
 * @param[in] top The max coordinate of the top side
 * @param[in] near The camera's nearest renderable object
 * @param[in] far The camera's furthest renderable object
 *
 * @return sa_mat4 The new projection matrix
 */
sa_mat4 sa_Mat4_Ortho(float left, float right, float bottom, float top, float near, float far);

/**
 * @fn sa_Mat4_Model_Matrix
 * @brief Returns a new model matrix from position, rotation, and scale
 *
 * @param[in] position The position vector (translation)
 * @param[in] rotation The rotation vector (angles in radians for X, Y, Z axes)
 * @param[in] scale The scale vector for X, Y, Z axes
 *
 * @return sa_mat4 A combined model matrix applying scale, rotation, and translation
 */
sa_mat4 sa_Mat4_Model_Matrix(sa_vec3 position, sa_vec3 rotation, sa_vec3 scale);

/**
 * @fn sa_Mat4_Rotate_X
 * @brief Returns a matrix rotated around the X-axis
 *
 * @param[in] mat The matrix to be rotated
 * @param[in] angle The angle in radians to rotate around the X-axis
 *
 * @return sa_mat4 The resulting rotated matrix
 */
sa_mat4 sa_Mat4_Rotate_X(sa_mat4 mat, float angle);

/**
 * @fn sa_Mat4_Rotate_Y
 * @brief Returns a matrix rotated around the Y-axis
 *
 * @param[in] mat The matrix to be rotated
 * @param[in] angle The angle in radians to rotate around the Y-axis
 *
 * @return sa_mat4 The resulting rotated matrix
 */
sa_mat4 sa_Mat4_Rotate_Y(sa_mat4 mat, float angle);

/**
 * @fn sa_Mat4_Rotate_Z
 * @brief Returns a matrix rotated around the Z-axis
 *
 * @param[in] mat The matrix to be rotated
 * @param[in] angle The angle in radians to rotate around the Z-axis
 *
 * @return sa_mat4 The resulting rotated matrix
 */
sa_mat4 sa_Mat4_Rotate_Z(sa_mat4 mat, float angle);

/**
 * @fn sa_Mat4_Scale
 * @brief Returns the new scaled matrix
 *
 * @param[in] sx Scaling factor along the X-axis
 * @param[in] sy Scaling factor along the Y-axis
 * @param[in] sz Scaling factor along the Z-axis
 *
 * @return sa_mat4 A matrix representing the scale transformation
 */
sa_mat4 sa_Mat4_Scale(float sx, float sy, float sz);

/**
 * @fn sa_Mat4_Translate
 * @brief Returns the new translation matrix
 *
 * @param[in] tx Translation along the X-axis
 * @param[in] ty Translation along the Y-axis
 * @param[in] tz Translation along the Z-axis
 *
 * @return sa_mat4 A matrix representing the translation transformation
 */
sa_mat4 sa_Mat4_Translate(float tx, float ty, float tz);

#endif
