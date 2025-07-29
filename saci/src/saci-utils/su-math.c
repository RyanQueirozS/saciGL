#include "saci-utils/su-math.h"
#include "saci-utils/su-general.h"

#include <math.h>
#include <string.h>

#define su_COLOR_8BIT_MAX (255.0f)
#define su_COLOR_8BIT_INVERSE_MAX (1.0f / su_COLOR_8BIT_MAX) // Used for Color related math

static double su__default_sqrt_s(double x) { // wrapps math.h sqrt func
    return sqrt(x);
}

static double su__default_tan_s(double x) { // wrapps math.h tan func
    return tan(x);
}

static struct {
    double (*sqrt_function)(double);
    double (*tan_function)(double);
} su_math_preferences = {
    .sqrt_function = su__default_sqrt_s,
    .tan_function = su__default_tan_s,
};

//------------------------------------------------------------------------------
// Vec3
//------------------------------------------------------------------------------

su_Vec3 su_vec3_subtract(su_Vec3 a, su_Vec3 b) {
    su_Vec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

su_Vec3 su_vec3_add(su_Vec3 a, su_Vec3 b) {
    su_Vec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

su_Vec3 su_vec3_scale(su_Vec3 v, float scalar) {
    su_Vec3 result = {v.x * scalar, v.y *= scalar, v.z *= scalar};
    return result;
}

su_Vec3 su_vec3_normalize(su_Vec3 v) {
    float mag = su_SCAST_TO_M(float)(su_math_preferences.sqrt_function(v.x * v.x + v.y * v.y + v.z * v.z));
    if (mag == 0.0f) {
        return (su_Vec3){0.0f, 0.0f, 0.0f};
    }
    su_Vec3 result = {v.x / mag, v.y / mag, v.z / mag};
    return result;
}

su_Vec3 su_vec3_cross(su_Vec3 a, su_Vec3 b) {
    su_Vec3 result = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    return result;
}

float su_vec3_dot(su_Vec3 a, su_Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

//------------------------------------------------------------------------------
// Color
//------------------------------------------------------------------------------

su_Color su_color_from_hex(su_U32 hex) {
    su_Color color;
    color.r = su_SCAST_TO_M(float)(((hex >> 24)) & 0xFF) * su_COLOR_8BIT_INVERSE_MAX;
    color.g = su_SCAST_TO_M(float)(((hex >> 16)) & 0xFF) * su_COLOR_8BIT_INVERSE_MAX;
    color.b = su_SCAST_TO_M(float)(((hex >> 8)) & 0xFF) * su_COLOR_8BIT_INVERSE_MAX;
    color.a = su_SCAST_TO_M(float)((hex & 0xFF)) * su_COLOR_8BIT_INVERSE_MAX;
    return color;
}

su_Color su_color_from_u8(su_U8 r, su_U8 g, su_U8 b, su_U8 a) {
    su_Color color;
    color.r = su_SCAST_TO_M(float)(r) * su_COLOR_8BIT_INVERSE_MAX;
    color.g = su_SCAST_TO_M(float)(g) * su_COLOR_8BIT_INVERSE_MAX;
    color.b = su_SCAST_TO_M(float)(b) * su_COLOR_8BIT_INVERSE_MAX;
    color.a = su_SCAST_TO_M(float)(a) * su_COLOR_8BIT_INVERSE_MAX;
    return color;
}

su_U32 su_color_to_hex(su_Color color) {
    su_U32 hex = ((su_U32)(color.r * su_COLOR_8BIT_MAX) << 24) |
                 ((su_U32)(color.g * su_COLOR_8BIT_MAX) << 16) |
                 ((su_U32)(color.b * su_COLOR_8BIT_MAX) << 8) |
                 (su_U32)(color.a * su_COLOR_8BIT_MAX);
    return hex;
}
//------------------------------------------------------------------------------
// Mat4
//------------------------------------------------------------------------------

su_Mat4 su_mat4_multiply(su_Mat4 a, su_Mat4 b) {
    su_Mat4 result;
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

su_Mat4 su_mat4_look_at(su_Vec3 eye, su_Vec3 center, su_Vec3 up) {
    su_Vec3 f = su_vec3_normalize(su_vec3_subtract(center, eye));
    su_Vec3 s = su_vec3_normalize(su_vec3_cross(f, up));
    su_Vec3 u = su_vec3_cross(s, f);

    su_Mat4 result = su_IDENTITY_MAT4;
    result.data[0][0] = s.x;
    result.data[0][1] = u.x;
    result.data[0][2] = -f.x;
    result.data[1][0] = s.y;
    result.data[1][1] = u.y;
    result.data[1][2] = -f.y;
    result.data[2][0] = s.z;
    result.data[2][1] = u.z;
    result.data[2][2] = -f.z;
    result.data[3][0] = -su_vec3_dot(s, eye);
    result.data[3][1] = -su_vec3_dot(u, eye);
    result.data[3][2] = su_vec3_dot(f, eye);

    return result;
}

// TODO evaluate if there is a way not to use tan operations
su_Mat4 su_mat4_perspective(float fov, float aspect, float near, float far) {
    su_Mat4 result = {0};
    float tan_half_fov = su_SCAST_TO_M(float)(su_math_preferences.tan_function(su_DEG_TO_RAD_M(fov) / 2.0f));

    result.data[0][0] = 1.0f / (aspect * tan_half_fov);
    result.data[1][1] = 1.0f / tan_half_fov;
    result.data[2][2] = -(far + near) / (far - near);
    result.data[2][3] = -1.0f;
    result.data[3][2] = -(2.0f * far * near) / (far - near);

    return result;
}

su_Mat4 su_mat4_ortho(float left, float right, float bottom, float top, float near, float far) {
    su_Mat4 result = {0};

    result.data[0][0] = 2.0f / (right - left);
    result.data[1][1] = 2.0f / (top - bottom);
    result.data[2][2] = -2.0f / (far - near);
    result.data[3][0] = -(right + left) / (right - left);
    result.data[3][1] = -(top + bottom) / (top - bottom);
    result.data[3][2] = -(far + near) / (far - near);
    result.data[3][3] = 1.0f;

    return result;
}

su_Mat4 su_mat4_model_matrix_rts(su_Vec3 position, su_Vec3 rotation, su_Vec3 scale) {
    su_Mat4 scale_mat = su_mat4_scale(scale.x, scale.y, scale.z);

    su_Mat4 rotation_x =
        su_mat4_rotate_z(su_IDENTITY_MAT4, rotation.x); // Rotate around X-axis
    su_Mat4 rotation_y =
        su_mat4_rotate_y(su_IDENTITY_MAT4, rotation.y); // Rotate around Y-axis
    su_Mat4 rotation_z =
        su_mat4_rotate_z(su_IDENTITY_MAT4, rotation.z); // Rotate around Z-axis

    su_Mat4 rotation_mat = su_mat4_multiply(rotation_z, su_mat4_multiply(rotation_y, rotation_x));

    su_Mat4 translation_mat = su_mat4_translate(position.x, position.y, position.z);

    su_Mat4 model_matrix = su_mat4_multiply(translation_mat, su_mat4_multiply(rotation_mat, scale_mat));

    return model_matrix;
}

su_Mat4 su_mat4_model_matrix_trs(su_Vec3 position, su_Vec3 rotation, su_Vec3 scale) {
    su_Mat4 scale_mat = su_mat4_scale(scale.x, scale.y, scale.z);

    su_Mat4 rotation_x = su_mat4_rotate_x(su_IDENTITY_MAT4, rotation.x);
    su_Mat4 rotation_y = su_mat4_rotate_y(su_IDENTITY_MAT4, rotation.y);
    su_Mat4 rotation_z = su_mat4_rotate_z(su_IDENTITY_MAT4, rotation.z);

    su_Mat4 rotation_mat = su_mat4_multiply(rotation_z, su_mat4_multiply(rotation_y, rotation_x));

    su_Mat4 translation_mat = su_mat4_translate(position.x, position.y, position.z);

    // Rotate first -> scale -> then translate
    su_Mat4 model_matrix = su_mat4_multiply(rotation_mat, su_mat4_multiply(translation_mat, scale_mat));
    return model_matrix;
}

su_Mat4 su_mat4_rotate_x(su_Mat4 mat, float angle) {
    su_Mat4 rotation = su_IDENTITY_MAT4;
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);

    rotation.data[1][1] = cos_a;
    rotation.data[1][2] = -sin_a;
    rotation.data[2][1] = sin_a;
    rotation.data[2][2] = cos_a;

    return su_mat4_multiply(mat, rotation);
}

su_Mat4 su_mat4_rotate_y(su_Mat4 mat, float angle) {
    su_Mat4 rotation = su_IDENTITY_MAT4;
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);

    rotation.data[0][0] = cos_a;
    rotation.data[0][2] = sin_a;
    rotation.data[2][0] = -sin_a;
    rotation.data[2][2] = cos_a;

    return su_mat4_multiply(mat, rotation);
}

su_Mat4 su_mat4_rotate_z(su_Mat4 mat, float angle) {
    su_Mat4 rotation = su_IDENTITY_MAT4;
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);

    rotation.data[0][0] = cos_a;
    rotation.data[0][1] = -sin_a;
    rotation.data[1][0] = sin_a;
    rotation.data[1][1] = cos_a;

    return su_mat4_multiply(mat, rotation);
}

su_Mat4 su_mat4_scale(float sx, float sy, float sz) {
    su_Mat4 result = su_IDENTITY_MAT4;
    result.data[0][0] = sx; // Scale in x direction
    result.data[1][1] = sy; // Scale in y direction
    result.data[2][2] = sz; // Scale in z direction
    return result;
}

su_Mat4 su_mat4_translate(float tx, float ty, float tz) {
    su_Mat4 result = su_IDENTITY_MAT4;
    result.data[3][0] = tx; // Translate in x direction
    result.data[3][1] = ty; // Translate in y direction
    result.data[3][2] = tz; // Translate in z direction
    return result;
}
