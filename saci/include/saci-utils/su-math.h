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

#ifndef SACI_UTILS_SU_MATH_H
#define SACI_UTILS_SU_MATH_H

#include "saci-utils/su-types.h"

/* === Constants === */

/**
 * @define su_PI
 * @brief PI declaration up to 14 digits
 */
#define su_PI 3.141592653589793f

/**
 * @define su_DEG2RAD_MULT
 * @brief Multiplication constant value for degree to radian operations
 */
#define su_DEG2RAD_MULT (su_PI / 180.0f)

/**
 * @define su_DEG_TO_RAD_M
 * @brief Macro that returns a degree from a radian
 */
#define su_DEG_TO_RAD_M(x) ((x) * su_DEG2RAD_MULT)

/**
 * @define su_RAD2DEG_MULT
 * @brief Multiplication constant value for radian to degree operations
 */
#define su_RAD2DEG_MULT (180.0f / SACI_PI)

/**
 * @define su_RAD_TO_DEG_M
 * @brief Macro that returns a radian from a degree
 */
#define su_RAD_TO_DEG_M(x) ((x) * su_RAD2DEG_MULT)

static const su_Mat4 su_IDENTITY_MAT4 = {
    .data = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    },
};

/* === Vec3 Operations === */

/**
 * @fn su_vec3_subtract
 * @brief Returns a new vector from a vector subtracted from another
 *
 * @param[in] a The base vector that will be subtracted
 * @param[in] b The vetor that will subtract
 *
 * @return su_Vec3 The result of the subtraction
 */
su_Vec3 su_vec3_subtract(su_Vec3 a, su_Vec3 b);

/**
 * @fn su_vec3_add
 * @brief Returns a new vector from a vector added from another
 *
 * @param[in] a The vector that will be added
 * @param[in] b The vetor that will added
 *
 * @return su_Vec3 The result of the subtraction
 */
su_Vec3 su_vec3_add(su_Vec3 a, su_Vec3 b);

/**
 * @fn su_vec3_scale
 * @brief Returns a new scaled vector
 *
 * @param[in] a The vector that will be added
 * @param[in] b The vetor that will added
 *
 * @return su_Vec3 The result of the scalation
 */
su_Vec3 su_vec3_scale(su_Vec3 v, float scalar);

/**
 * @fn su_vec3_normalize
 * @brief Returns a normalized vector to unit length (1)
 *
 * @param[in] v The vector to normalize
 *
 * @return su_Vec3 The normalized vector with a length of 1
 */
su_Vec3 su_vec3_normalize(su_Vec3 v);

/**
 * @fn su_vec3_cross
 * @brief Returns the computed cross product of two vectors
 *
 * @param[in] a The first vector
 * @param[in] b The second vector
 *
 * @return su_Vec3 The result of the cross product of vectors a and b
 */

su_Vec3 su_vec3_cross(su_Vec3 a, su_Vec3 b);

/**
 * @fn su_vec3_dot
 * @brief Returns the computed dot product of two vectors
 *
 * @param[in] a The first vector
 * @param[in] b The second vector
 *
 * @return float The result of the dot product of vectors a and b
 */
float su_vec3_dot(su_Vec3 a, su_Vec3 b);

/* === Color Operations === */

/**
 * @fn su_color_from_hex
 * @brief Returns a new su_Color from a given hex code
 *
 * @param[in] hex The hex code 0x12345678
 *
 * @return su_Color The new color based on the hex code
 */
su_Color su_color_from_hex(su_U32 hex);

/**
 * @fn su_color_from_u8
 * @brief Returns a new color from given rgba values
 *
 * @param[in] r The r component of the color
 * @param[in] g The g component of the color
 * @param[in] b The b component of the color
 * @param[in] a The a component of the color
 *
 * @return su_Color The new color based on the rgba value
 */
su_Color su_color_from_u8(su_U8 r, su_U8 g, su_U8 b, su_U8 a);

/**
 * @fn su_color_to_hex
 * @brief Returns a new hex code from a color
 *
 * @param[in] color The color to be evaluated
 *
 * @return su_U32 The new hex code
 */
su_U32 su_color_to_hex(su_Color color);

/* === Mat Operations === */

/**
 * @fn su_mat4_multiply
 * @brief Returns a Multiplied matrix given another
 *
 * @param[in] a The matrix to be multiplied
 * @param[in] b The matrix to be multiplied
 *
 * @return su_Mat4 The multiplied matrix
 */

su_Mat4 su_mat4_multiply(su_Mat4 a, su_Mat4 b);

/**
 * @fn su_mat4_look_at
 * @brief Returns a "view" matrix
 *
 * @param[in] eye The camera's position in world space
 * @param[in] center The target point or the "look-at" point
 * @param[in] up The "up" direction for the camera
 *
 * @return su_Mat4 The new view matrix
 */
su_Mat4 su_mat4_look_at(su_Vec3 eye, su_Vec3 center, su_Vec3 up);

/**
 * @fn su_mat4_perspective
 * @brief Returns a perspective "projection" matrix
 *
 * @param[in] fov The camera's fov
 * @param[in] aspect The camera's aspect ratio
 * @param[in] near The camera's nearest renderable object
 * @param[in] far The camera's furthest renderable object
 *
 * @return su_Mat4 The new projection matrix
 */
su_Mat4 su_mat4_perspective(float fov, float aspect, float near, float far);

/**
 * @fn su_mat4_ortho
 * @brief Returns an orthogonal "projection" matrix
 *
 * @param[in] left The max coordinate of the left side
 * @param[in] right The max coordinate of the right side
 * @param[in] bottom The max coordinate of the bottom side
 * @param[in] top The max coordinate of the top side
 * @param[in] near The camera's nearest renderable object
 * @param[in] far The camera's furthest renderable object
 *
 * @return su_Mat4 The new projection matrix
 */
su_Mat4 su_mat4_ortho(float left, float right, float bottom, float top, float near, float far);

/**
 * @fn su_mat4_model_matrix_trs
 * @brief Returns a new model matrix from position, rotation, and scale.
 * It follows a `rotation * translation * scale` order.
 *
 * @param[in] position The position vector (translation)
 * @param[in] rotation The rotation vector (angles in radians for X, Y, Z axes)
 * @param[in] scale The scale vector for X, Y, Z axes
 *
 * @return su_Mat4 A combined model matrix applying scale, rotation, and translation
 */
su_Mat4 su_mat4_model_matrix_rts(su_Vec3 position, su_Vec3 rotation, su_Vec3 scale);

/**
 * @fn su_mat4_model_matrix_trs
 * @brief Returns a new model matrix from position, rotation, and scale.
 * It follows a `translation * rotation * scale` order.
 *
 * @param[in] position The position vector (translation)
 * @param[in] rotation The rotation vector (angles in radians for X, Y, Z axes)
 * @param[in] scale The scale vector for X, Y, Z axes
 *
 * @return su_Mat4 A combined model matrix applying scale, rotation, and translation
 */
su_Mat4 su_mat4_model_matrix_trs(su_Vec3 position, su_Vec3 rotation, su_Vec3 scale);

/**
 * @fn su_mat4_rotate_x
 * @brief Returns a matrix rotated around the X-axis
 *
 * @param[in] mat The matrix to be rotated
 * @param[in] angle The angle in radians to rotate around the X-axis
 *
 * @return su_Mat4 The resulting rotated matrix
 */
su_Mat4 su_mat4_rotate_x(su_Mat4 mat, float angle);

/**
 * @fn su_mat4_rotate_y
 * @brief Returns a matrix rotated around the Y-axis
 *
 * @param[in] mat The matrix to be rotated
 * @param[in] angle The angle in radians to rotate around the Y-axis
 *
 * @return su_Mat4 The resulting rotated matrix
 */
su_Mat4 su_mat4_rotate_y(su_Mat4 mat, float angle);

/**
 * @fn su_mat4_rotate_z
 * @brief Returns a matrix rotated around the Z-axis
 *
 * @param[in] mat The matrix to be rotated
 * @param[in] angle The angle in radians to rotate around the Z-axis
 *
 * @return su_Mat4 The resulting rotated matrix
 */
su_Mat4 su_mat4_rotate_z(su_Mat4 mat, float angle);

/**
 * @fn su_mat4_scale
 * @brief Returns the new scaled matrix
 *
 * @param[in] sx Scaling factor along the X-axis
 * @param[in] sy Scaling factor along the Y-axis
 * @param[in] sz Scaling factor along the Z-axis
 *
 * @return su_Mat4 A matrix representing the scale transformation
 */
su_Mat4 su_mat4_scale(float sx, float sy, float sz);

/**
 * @fn su_mat4_translate
 * @brief Returns the new translation matrix
 *
 * @param[in] tx Translation along the X-axis
 * @param[in] ty Translation along the Y-axis
 * @param[in] tz Translation along the Z-axis
 *
 * @return su_Mat4 A matrix representing the translation transformation
 */
su_Mat4 su_mat4_translate(float tx, float ty, float tz);

#endif // SACI_UTILS_SU_MATH_H
