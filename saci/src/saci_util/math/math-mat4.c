#include "saci_util/math.h"

#include "saci_util/types.h"
#include "saci_util/internal/general.h"

// TODO remove <math.h> use the math.h from saci
#include <math.h>
#include <string.h>

SaciMat4 saci_mat4_multiply(SaciMat4 a, SaciMat4 b)
{
    SaciMat4 result;
    memset(result.data, 0, sizeof(result.data));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result.data[i][j] += a.data[i][k] * b.data[k][j];
            }
        }
    }
    return result;
}

SaciMat4 saci_mat4_look_at(SaciVec3 eye, SaciVec3 center, SaciVec3 up)
{
    SaciVec3 f = saci_vec3_normalize(saci_vec3_subtract(center, eye));
    SaciVec3 s = saci_vec3_normalize(saci_vec3_cross(f, up));
    SaciVec3 u = saci_vec3_cross(s, f);

    SaciMat4 result = SACI_G_IDENTITY_MAT4;
    result.data[0][0] = s.x;
    result.data[0][1] = u.x;
    result.data[0][2] = -f.x;
    result.data[1][0] = s.y;
    result.data[1][1] = u.y;
    result.data[1][2] = -f.y;
    result.data[2][0] = s.z;
    result.data[2][1] = u.z;
    result.data[2][2] = -f.z;
    result.data[3][0] = -saci_vec3_dot(s, eye);
    result.data[3][1] = -saci_vec3_dot(u, eye);
    result.data[3][2] = saci_vec3_dot(f, eye);

    return result;
}

// TODO evaluate if there is a way not to use tan operations
SaciMat4 saci_mat4_perspective(float fov, float aspect, float near, float far)
{
    SaciMat4 result = {0};
    float tan_half_fov = SACI_CAST_M(float)(saci_math_tan(su_DEG_TO_RAD_M(fov) / 2.0f));

    result.data[0][0] = 1.0f / (aspect * tan_half_fov);
    result.data[1][1] = 1.0f / tan_half_fov;
    result.data[2][2] = -(far + near) / (far - near);
    result.data[2][3] = -1.0f;
    result.data[3][2] = -(2.0f * far * near) / (far - near);

    return result;
}

SaciMat4 saci_mat4_ortho(float left, float right, float bottom, float top, float near, float far)
{
    SaciMat4 result = {0};

    result.data[0][0] = 2.0f / (right - left);
    result.data[1][1] = 2.0f / (top - bottom);
    result.data[2][2] = -2.0f / (far - near);
    result.data[3][0] = -(right + left) / (right - left);
    result.data[3][1] = -(top + bottom) / (top - bottom);
    result.data[3][2] = -(far + near) / (far - near);
    result.data[3][3] = 1.0f;

    return result;
}

SaciMat4 saci_mat4_model_matrix_rts(SaciVec3 position, SaciVec3 rotation, SaciVec3 scale)
{
    SaciMat4 scale_mat = saci_mat4_scale(scale.x, scale.y, scale.z);

    SaciMat4 rotation_x = saci_mat4_rotate_x(SACI_G_IDENTITY_MAT4, rotation.x);
    SaciMat4 rotation_y = saci_mat4_rotate_y(SACI_G_IDENTITY_MAT4, rotation.y);
    SaciMat4 rotation_z = saci_mat4_rotate_z(SACI_G_IDENTITY_MAT4, rotation.z);

    SaciMat4 rotation_mat = saci_mat4_multiply(rotation_z,
                                               saci_mat4_multiply(rotation_y, rotation_x));

    SaciMat4 translation_mat = saci_mat4_translate(position.x, position.y, position.z);

    SaciMat4 model_matrix = saci_mat4_multiply(
        rotation_mat,
        saci_mat4_multiply(translation_mat, scale_mat));

    return model_matrix;
}

SaciMat4 saci_mat4_model_matrix_trs(SaciVec3 position, SaciVec3 rotation, SaciVec3 scale)
{
    SaciMat4 scale_mat = saci_mat4_scale(scale.x, scale.y, scale.z);

    SaciMat4 rotation_x = saci_mat4_rotate_x(SACI_G_IDENTITY_MAT4, rotation.x);
    SaciMat4 rotation_y = saci_mat4_rotate_y(SACI_G_IDENTITY_MAT4, rotation.y);
    SaciMat4 rotation_z = saci_mat4_rotate_z(SACI_G_IDENTITY_MAT4, rotation.z);

    SaciMat4 rotation_mat = saci_mat4_multiply(rotation_z,
                                               saci_mat4_multiply(rotation_y, rotation_x));

    SaciMat4 translation_mat = saci_mat4_translate(position.x, position.y, position.z);

    SaciMat4 model_matrix = saci_mat4_multiply(
        translation_mat,
        saci_mat4_multiply(rotation_mat, scale_mat));

    return model_matrix;
}

SaciMat4 saci_mat4_rotate_x(SaciMat4 mat, float angle)
{
    SaciMat4 rotation = SACI_G_IDENTITY_MAT4;
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);

    rotation.data[1][1] = cos_a;
    rotation.data[1][2] = -sin_a;
    rotation.data[2][1] = sin_a;
    rotation.data[2][2] = cos_a;

    return saci_mat4_multiply(mat, rotation);
}

SaciMat4 saci_mat4_rotate_y(SaciMat4 mat, float angle)
{
    SaciMat4 rotation = SACI_G_IDENTITY_MAT4;
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);

    rotation.data[0][0] = cos_a;
    rotation.data[0][2] = sin_a;
    rotation.data[2][0] = -sin_a;
    rotation.data[2][2] = cos_a;

    return saci_mat4_multiply(mat, rotation);
}

SaciMat4 saci_mat4_rotate_z(SaciMat4 mat, float angle)
{
    SaciMat4 rotation = SACI_G_IDENTITY_MAT4;
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);

    rotation.data[0][0] = cos_a;
    rotation.data[0][1] = -sin_a;
    rotation.data[1][0] = sin_a;
    rotation.data[1][1] = cos_a;

    return saci_mat4_multiply(mat, rotation);
}

SaciMat4 saci_mat4_scale(float sx, float sy, float sz)
{
    SaciMat4 result = SACI_G_IDENTITY_MAT4;
    result.data[0][0] = sx; // Scale in x direction
    result.data[1][1] = sy; // Scale in y direction
    result.data[2][2] = sz; // Scale in z direction
    return result;
}

SaciMat4 saci_mat4_translate(float tx, float ty, float tz)
{
    SaciMat4 result = SACI_G_IDENTITY_MAT4;
    result.data[3][0] = tx; // Translate in x direction
    result.data[3][1] = ty; // Translate in y direction
    result.data[3][2] = tz; // Translate in z direction
    return result;
}
