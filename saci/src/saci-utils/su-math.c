#include "saci-utils/su-math.h"
#include "saci-utils/su-general.h"

#include <math.h>
#include <string.h>

#define sa_COLOR_8BIT_MAX 255.0f
#define sa_COLOR_8BIT_INVERSE_MAX 1.0f / sa_COLOR_8BIT_MAX // Used for Color related math

static double __sa_default_sqrt(double x) { // wrapps math.h sqrt func
    return sqrt(x);
}

static double __sa_default_tan(double x) { // wrapps math.h tan func
    return tan(x);
}

static struct {
    double (*sqrt_function)(double);
    double (*tan_function)(double);
} __sa_math_preferences = {
    .sqrt_function = __sa_default_sqrt,
    .tan_function = __sa_default_tan,
};

//------------------------------------------------------------------------------
// Vec3
//------------------------------------------------------------------------------

sa_vec3 sa_Vec3_Subtract(sa_vec3 a, sa_vec3 b) {
    sa_vec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

sa_vec3 sa_Vec3_Add(sa_vec3 a, sa_vec3 b) {
    sa_vec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

sa_vec3 sa_Vec3_Scale(sa_vec3 v, float scalar) {
    sa_vec3 result = {v.x * scalar, v.y *= scalar, v.z *= scalar};
    return result;
}

sa_vec3 sa_Vec3_Normalize(sa_vec3 v) {
    float mag = sa_SCAST_TO_m(float)(__sa_math_preferences.sqrt_function(v.x * v.x + v.y * v.y + v.z * v.z));
    if (mag == 0.0f) {
        return (sa_vec3){0.0f, 0.0f, 0.0f};
    }
    sa_vec3 result = {v.x / mag, v.y / mag, v.z / mag};
    return result;
}

sa_vec3 sa_Vec3_Cross(sa_vec3 a, sa_vec3 b) {
    sa_vec3 result = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    return result;
}

float sa_Vec3_Dot(sa_vec3 a, sa_vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

//------------------------------------------------------------------------------
// Color
//------------------------------------------------------------------------------

sa_color sa_Color_From_Hex(sa_u32 hex) {
    sa_color color;
    color.r = ((hex >> 24) & 0xFF) * sa_SCAST_TO_m(sa_u32)(sa_COLOR_8BIT_INVERSE_MAX);
    color.g = ((hex >> 16) & 0xFF) * sa_SCAST_TO_m(sa_u32)(sa_COLOR_8BIT_INVERSE_MAX);
    color.b = ((hex >> 8) & 0xFF) * sa_SCAST_TO_m(sa_u32)(sa_COLOR_8BIT_INVERSE_MAX);
    color.a = (hex & 0xFF) * sa_SCAST_TO_m(sa_u32)(sa_COLOR_8BIT_INVERSE_MAX);
    return color;
}

sa_color sa_Color_From_U8(sa_u8 r, sa_u8 g, sa_u8 b, sa_u8 a) {
    sa_color color;
    color.r = r * sa_COLOR_8BIT_INVERSE_MAX;
    color.g = g * sa_COLOR_8BIT_INVERSE_MAX;
    color.b = b * sa_COLOR_8BIT_INVERSE_MAX;
    color.a = a * sa_COLOR_8BIT_INVERSE_MAX;
    return color;
}

sa_u32 sa_Color_To_Hex(sa_color color) {
    sa_u32 hex = ((sa_u32)(color.r * sa_COLOR_8BIT_MAX) << 24) |
                 ((sa_u32)(color.g * sa_COLOR_8BIT_MAX) << 16) |
                 ((sa_u32)(color.b * sa_COLOR_8BIT_MAX) << 8) |
                 (sa_u32)(color.a * sa_COLOR_8BIT_MAX);
    return hex;
}
//------------------------------------------------------------------------------
// Mat4
//------------------------------------------------------------------------------

sa_mat4 sa_Mat4_Multiply(sa_mat4 a, sa_mat4 b) {
    sa_mat4 result;
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

sa_mat4 sa_Mat4_Identity(void) {
    sa_mat4 result = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
    return result;
}

sa_mat4 sa_Mat4_Look_At(sa_vec3 eye, sa_vec3 center, sa_vec3 up) {
    sa_vec3 f = sa_Vec3_Normalize(sa_Vec3_Subtract(center, eye));
    sa_vec3 s = sa_Vec3_Normalize(sa_Vec3_Cross(f, up));
    sa_vec3 u = sa_Vec3_Cross(s, f);

    sa_mat4 result = sa_Mat4_Identity();
    result.data[0][0] = s.x;
    result.data[0][1] = u.x;
    result.data[0][2] = -f.x;
    result.data[1][0] = s.y;
    result.data[1][1] = u.y;
    result.data[1][2] = -f.y;
    result.data[2][0] = s.z;
    result.data[2][1] = u.z;
    result.data[2][2] = -f.z;
    result.data[3][0] = -sa_Vec3_Dot(s, eye);
    result.data[3][1] = -sa_Vec3_Dot(u, eye);
    result.data[3][2] = sa_Vec3_Dot(f, eye);

    return result;
}

// TODO evaluate if there is a way not to use tan operations
sa_mat4 sa_Mat4_Perspective(float fov, float aspect, float near, float far) {
    sa_mat4 result = {0};
    float tanHalfFov = sa_SCAST_TO_m(float)(__sa_math_preferences.tan_function(sa_Deg_To_Rad_m(fov) / 2.0f));

    result.data[0][0] = 1.0f / (aspect * tanHalfFov);
    result.data[1][1] = 1.0f / tanHalfFov;
    result.data[2][2] = -(far + near) / (far - near);
    result.data[2][3] = -1.0f;
    result.data[3][2] = -(2.0f * far * near) / (far - near);

    return result;
}

sa_mat4 sa_Mat4_Ortho(float left, float right, float bottom, float top, float near, float far) {
    sa_mat4 result = {0};

    result.data[0][0] = 2.0f / (right - left);
    result.data[1][1] = 2.0f / (top - bottom);
    result.data[2][2] = -2.0f / (far - near);
    result.data[3][0] = -(right + left) / (right - left);
    result.data[3][1] = -(top + bottom) / (top - bottom);
    result.data[3][2] = -(far + near) / (far - near);
    result.data[3][3] = 1.0f;

    return result;
}

sa_mat4 sa_Mat4_Rotate_X(sa_mat4 mat, float angle) {
    sa_mat4 rotation = sa_Mat4_Identity();
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.data[1][1] = cosA;
    rotation.data[1][2] = -sinA;
    rotation.data[2][1] = sinA;
    rotation.data[2][2] = cosA;

    return sa_Mat4_Multiply(mat, rotation);
}

sa_mat4 sa_Mat4_Rotate_Y(sa_mat4 mat, float angle) {
    sa_mat4 rotation = sa_Mat4_Identity();
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.data[0][0] = cosA;
    rotation.data[0][2] = sinA;
    rotation.data[2][0] = -sinA;
    rotation.data[2][2] = cosA;

    return sa_Mat4_Multiply(mat, rotation);
}

sa_mat4 sa_Mat4_Rotate_Z(sa_mat4 mat, float angle) {
    sa_mat4 rotation = sa_Mat4_Identity();
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.data[0][0] = cosA;
    rotation.data[0][1] = -sinA;
    rotation.data[1][0] = sinA;
    rotation.data[1][1] = cosA;

    return sa_Mat4_Multiply(mat, rotation);
}

sa_mat4 sa_Mat4_Scale(float sx, float sy, float sz) {
    sa_mat4 result = sa_Mat4_Identity();
    result.data[0][0] = sx; // Scale in x direction
    result.data[1][1] = sy; // Scale in y direction
    result.data[2][2] = sz; // Scale in z direction
    return result;
}

sa_mat4 sa_Mat4_Translate(float tx, float ty, float tz) {
    sa_mat4 result = sa_Mat4_Identity();
    result.data[3][0] = tx; // Translate in x direction
    result.data[3][1] = ty; // Translate in y direction
    result.data[3][2] = tz; // Translate in z direction
    return result;
}

sa_mat4 sa_Mat4_Model_Matrix(sa_vec3 position, sa_vec3 rotation, sa_vec3 scale) {
    sa_mat4 scaleMat = sa_Mat4_Scale(scale.x, scale.y, scale.z);

    sa_mat4 rotationX =
        sa_Mat4_Rotate_Z(sa_Mat4_Identity(), rotation.x); // Rotate around X-axis
    sa_mat4 rotationY =
        sa_Mat4_Rotate_Y(sa_Mat4_Identity(), rotation.y); // Rotate around Y-axis
    sa_mat4 rotationZ =
        sa_Mat4_Rotate_Z(sa_Mat4_Identity(), rotation.z); // Rotate around Z-axis

    sa_mat4 rotationMat = sa_Mat4_Multiply(rotationZ, sa_Mat4_Multiply(rotationY, rotationX));

    sa_mat4 translationMat = sa_Mat4_Translate(position.x, position.y, position.z);

    sa_mat4 modelMatrix =
        sa_Mat4_Multiply(translationMat, sa_Mat4_Multiply(rotationMat, scaleMat));

    return modelMatrix;
}
