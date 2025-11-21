#ifndef SACI_UTILS_MATH_SU_MATH_MAT_H
#define SACI_UTILS_MATH_SU_MATH_MAT_H

#include "./su-math-types.h"

static const su_Mat4 su_IDENTITY_MAT4 = {
    .data = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    },
};

#define su_MAT4_FMT "{{%.3f %.3f %.3f %.3f}, {%.3f %.3f %.3f %.3f}, {%.3f %.3f %.3f %.3f}, {%.3f %.3f %.3f %.3f}}"
#define su_MAT4_FMT_ARGS(m)                                             \
    (m).data[0][0], (m).data[0][1], (m).data[0][2], (m).data[0][3],     \
        (m).data[1][0], (m).data[1][1], (m).data[1][2], (m).data[1][3], \
        (m).data[2][0], (m).data[2][1], (m).data[2][2], (m).data[2][3], \
        (m).data[3][0], (m).data[3][1], (m).data[3][2], (m).data[3][3]

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

#endif // SACI_UTILS_MATH_SU_MATH_MAT_H
