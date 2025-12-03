#ifndef SACI_UTILS_MATH_SU_MATH_H
#define SACI_UTILS_MATH_SU_MATH_H

#include "./defines.h"
#include "./types.h"

/* === Constants === */

#define su_PI 3.141592653589793f

#define su_DEG2RAD_MULT (su_PI / 180.0f)

#define su_DEG_TO_RAD_M(x) ((x) * su_DEG2RAD_MULT)

#define su_RAD2DEG_MULT (180.0f / SACI_PI)

#define su_RAD_TO_DEG_M(x) ((x) * su_RAD2DEG_MULT)

// Types

typedef struct {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
    float z; /**< Z compoonent */
} SaciVec3;

typedef struct {
    float data[4][4]; /**< 4x4 matrix of floats */
} SaciMat4;

typedef struct {
    float r; /**< Red compoonent */
    float g; /**< Green compoonent */
    float b; /**< Blue compoonent */
    float a; /**< Alpha (transparency) compoonent */
} SaciColor;

// Preferences

double saci_math_sqrt(double val);

double saci_math_tan(double val);

// Vec3

#define SACI_VEC3_FMT "x:%f y:%f z:%f"
#define SACI_VEC3_FMT_ARGS(v) (v).x, (v).y, (v).z

SA_API SaciVec3 saci_vec3_subtract(SaciVec3 a, SaciVec3 b);

SA_API SaciVec3 saci_vec3_add(SaciVec3 a, SaciVec3 b);

SA_API SaciVec3 saci_vec3_scale(SaciVec3 v, float scalar);

SA_API SaciVec3 saci_vec3_normalize(SaciVec3 v);

SA_API SaciVec3 saci_vec3_cross(SaciVec3 a, SaciVec3 b);

SA_API float saci_vec3_dot(SaciVec3 a, SaciVec3 b);

// Color

#define SACI_COLOR_8BIT_MAX (255.0f)
#define SACI_COLOR_8BIT_INVERSE_MAX (1.0f / su_COLOR_8BIT_MAX)

SA_API SaciColor saci_color_from_hex(SaciU32 hex);

SA_API SaciColor saci_color_from_u8(SaciU8 r, SaciU8 g, SaciU8 b, SaciU8 a);

SA_API SaciU32 saci_color_to_hex(SaciColor color);

// Mat4

static const SaciMat4 SACI_G_IDENTITY_MAT4 = {
    .data = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
    },
};

#define SACI_MAT4_FMT "{{%.3f %.3f %.3f %.3f}, {%.3f %.3f %.3f %.3f}, {%.3f %.3f %.3f %.3f}, {%.3f %.3f %.3f %.3f}}"
#define SACI_MAT4_FMT_ARGS(m)                                           \
    (m).data[0][0], (m).data[0][1], (m).data[0][2], (m).data[0][3],     \
        (m).data[1][0], (m).data[1][1], (m).data[1][2], (m).data[1][3], \
        (m).data[2][0], (m).data[2][1], (m).data[2][2], (m).data[2][3], \
        (m).data[3][0], (m).data[3][1], (m).data[3][2], (m).data[3][3]

SaciMat4 saci_mat4_multiply(SaciMat4 a, SaciMat4 b);

SaciMat4 saci_mat4_look_at(SaciVec3 eye, SaciVec3 center, SaciVec3 up);

SaciMat4 saci_mat4_perspective(float fov, float aspect, float near, float far);

SaciMat4 saci_mat4_ortho(float left, float right, float bottom, float top, float near, float far);

SaciMat4 saci_mat4_model_matrix_rts(SaciVec3 position, SaciVec3 rotation, SaciVec3 scale);

SaciMat4 saci_mat4_model_matrix_trs(SaciVec3 position, SaciVec3 rotation, SaciVec3 scale);

SaciMat4 saci_mat4_rotate_x(SaciMat4 mat, float angle);

SaciMat4 saci_mat4_rotate_y(SaciMat4 mat, float angle);

SaciMat4 saci_mat4_rotate_z(SaciMat4 mat, float angle);

SaciMat4 saci_mat4_scale(float sx, float sy, float sz);

SaciMat4 saci_mat4_translate(float tx, float ty, float tz);

#endif // SACI_UTILS_MATH_SU_MATH_H
