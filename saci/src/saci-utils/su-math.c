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
} __sa_math_preferences;

//------------------------------------------------------------------------------
// Init
//------------------------------------------------------------------------------

void sa_Math_Init(void) {
    __sa_math_preferences.sqrt_function = __sa_default_sqrt; // defaults the sqrt
                                                             // operation to C's math.h
                                                             // sqrt function'

    __sa_math_preferences.tan_function = __sa_default_tan;
}

//------------------------------------------------------------------------------
// Vec3
//------------------------------------------------------------------------------

sa_vec3_t sa_Vec3_Subtract(sa_vec3_t a, sa_vec3_t b) {
    sa_vec3_t result = {a.m_x - b.m_x, a.m_y - b.m_y, a.m_z - b.m_z};
    return result;
}

sa_vec3_t sa_Vec3_Add(sa_vec3_t a, sa_vec3_t b) {
    sa_vec3_t result = {a.m_x + b.m_x, a.m_y + b.m_y, a.m_z + b.m_z};
    return result;
}

sa_vec3_t sa_Vec3_Scale(sa_vec3_t v, float scalar) {
    sa_vec3_t result = {v.m_x * scalar, v.m_y *= scalar, v.m_z *= scalar};
    return result;
}

sa_vec3_t sa_Vec3_Normalize(sa_vec3_t v) {
    float mag = sa_SCAST_TO_m(float)(__sa_math_preferences.sqrt_function(v.m_x * v.m_x + v.m_y * v.m_y + v.m_z * v.m_z));
    if (mag == 0.0f) {
        return (sa_vec3_t){0.0f, 0.0f, 0.0f};
    }
    sa_vec3_t result = {v.m_x / mag, v.m_y / mag, v.m_z / mag};
    return result;
}

sa_vec3_t sa_Vec3_Cross(sa_vec3_t a, sa_vec3_t b) {
    sa_vec3_t result = {a.m_y * b.m_z - a.m_z * b.m_y, a.m_z * b.m_x - a.m_x * b.m_z, a.m_x * b.m_y - a.m_y * b.m_x};
    return result;
}

float sa_Vec3_Dot(sa_vec3_t a, sa_vec3_t b) {
    return a.m_x * b.m_x + a.m_y * b.m_y + a.m_z * b.m_z;
}

//------------------------------------------------------------------------------
// Color
//------------------------------------------------------------------------------

sa_color_t sa_Color_From_Hex(sa_u32_t hex) {
    sa_color_t color;
    color.m_r = ((hex >> 24) & 0xFF) * sa_SCAST_TO_m(sa_u32_t)(sa_COLOR_8BIT_INVERSE_MAX);
    color.m_g = ((hex >> 16) & 0xFF) * sa_SCAST_TO_m(sa_u32_t)(sa_COLOR_8BIT_INVERSE_MAX);
    color.m_b = ((hex >> 8) & 0xFF) * sa_SCAST_TO_m(sa_u32_t)(sa_COLOR_8BIT_INVERSE_MAX);
    color.m_a = (hex & 0xFF) * sa_SCAST_TO_m(sa_u32_t)(sa_COLOR_8BIT_INVERSE_MAX);
    return color;
}

sa_color_t sa_Color_From_U8(sa_u8_t r, sa_u8_t g, sa_u8_t b, sa_u8_t a) {
    sa_color_t color;
    color.m_r = r * sa_COLOR_8BIT_INVERSE_MAX;
    color.m_g = g * sa_COLOR_8BIT_INVERSE_MAX;
    color.m_b = b * sa_COLOR_8BIT_INVERSE_MAX;
    color.m_a = a * sa_COLOR_8BIT_INVERSE_MAX;
    return color;
}

sa_u32_t sa_Color_To_Hex(sa_color_t color) {
    sa_u32_t hex = ((sa_u32_t)(color.m_r * sa_COLOR_8BIT_MAX) << 24) |
                   ((sa_u32_t)(color.m_g * sa_COLOR_8BIT_MAX) << 16) |
                   ((sa_u32_t)(color.m_b * sa_COLOR_8BIT_MAX) << 8) |
                   (sa_u32_t)(color.m_a * sa_COLOR_8BIT_MAX);
    return hex;
}
//------------------------------------------------------------------------------
// Mat4
//------------------------------------------------------------------------------

sa_mat4_t sa_Mat4_Multiply(sa_mat4_t a, sa_mat4_t b) {
    sa_mat4_t result;
    memset(result.m_data, 0, sizeof(result.m_data));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result.m_data[i][j] += a.m_data[i][k] * b.m_data[k][j];
            }
        }
    }
    return result;
}

sa_mat4_t sa_Mat4_Identity(void) {
    sa_mat4_t result = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
    return result;
}

sa_mat4_t sa_Mat4_Look_At(sa_vec3_t eye, sa_vec3_t center, sa_vec3_t up) {
    sa_vec3_t f = sa_Vec3_Normalize(sa_Vec3_Subtract(center, eye));
    sa_vec3_t s = sa_Vec3_Normalize(sa_Vec3_Cross(f, up));
    sa_vec3_t u = sa_Vec3_Cross(s, f);

    sa_mat4_t result = sa_Mat4_Identity();
    result.m_data[0][0] = s.m_x;
    result.m_data[0][1] = u.m_x;
    result.m_data[0][2] = -f.m_x;
    result.m_data[1][0] = s.m_y;
    result.m_data[1][1] = u.m_y;
    result.m_data[1][2] = -f.m_y;
    result.m_data[2][0] = s.m_z;
    result.m_data[2][1] = u.m_z;
    result.m_data[2][2] = -f.m_z;
    result.m_data[3][0] = -sa_Vec3_Dot(s, eye);
    result.m_data[3][1] = -sa_Vec3_Dot(u, eye);
    result.m_data[3][2] = sa_Vec3_Dot(f, eye);

    return result;
}

sa_mat4_t sa_Mat4_Perspective(float fov, float aspect, float near, float far) {
    sa_mat4_t result = {0};
    float tanHalfFov = sa_SCAST_TO_m(float)(__sa_math_preferences.tan_function(sa_DEG2RAD_m(fov) / 2.0f));

    result.m_data[0][0] = 1.0f / (aspect * tanHalfFov);
    result.m_data[1][1] = 1.0f / tanHalfFov;
    result.m_data[2][2] = -(far + near) / (far - near);
    result.m_data[2][3] = -1.0f;
    result.m_data[3][2] = -(2.0f * far * near) / (far - near);

    return result;
}

sa_mat4_t sa_Mat4_Ortho(float left, float right, float bottom, float top, float near, float far) {
    sa_mat4_t result = {0};

    result.m_data[0][0] = 2.0f / (right - left);
    result.m_data[1][1] = 2.0f / (top - bottom);
    result.m_data[2][2] = -2.0f / (far - near);
    result.m_data[3][0] = -(right + left) / (right - left);
    result.m_data[3][1] = -(top + bottom) / (top - bottom);
    result.m_data[3][2] = -(far + near) / (far - near);
    result.m_data[3][3] = 1.0f;

    return result;
}

sa_mat4_t sa_Mat4_Rotate_X(sa_mat4_t mat, float angle) {
    sa_mat4_t rotation = sa_Mat4_Identity();
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.m_data[1][1] = cosA;
    rotation.m_data[1][2] = -sinA;
    rotation.m_data[2][1] = sinA;
    rotation.m_data[2][2] = cosA;

    return sa_Mat4_Multiply(mat, rotation);
}

sa_mat4_t sa_Mat4_Rotate_Y(sa_mat4_t mat, float angle) {
    sa_mat4_t rotation = sa_Mat4_Identity();
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.m_data[0][0] = cosA;
    rotation.m_data[0][2] = sinA;
    rotation.m_data[2][0] = -sinA;
    rotation.m_data[2][2] = cosA;

    return sa_Mat4_Multiply(mat, rotation);
}

sa_mat4_t sa_Mat4_Rotate_Z(sa_mat4_t mat, float angle) {
    sa_mat4_t rotation = sa_Mat4_Identity();
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.m_data[0][0] = cosA;
    rotation.m_data[0][1] = -sinA;
    rotation.m_data[1][0] = sinA;
    rotation.m_data[1][1] = cosA;

    return sa_Mat4_Multiply(mat, rotation);
}

sa_mat4_t sa_Mat4_Scale(float sx, float sy, float sz) {
    sa_mat4_t result = sa_Mat4_Identity();
    result.m_data[0][0] = sx; // Scale in x direction
    result.m_data[1][1] = sy; // Scale in y direction
    result.m_data[2][2] = sz; // Scale in z direction
    return result;
}

sa_mat4_t sa_Mat4_Translate(float tx, float ty, float tz) {
    sa_mat4_t result = sa_Mat4_Identity();
    result.m_data[3][0] = tx; // Translate in x direction
    result.m_data[3][1] = ty; // Translate in y direction
    result.m_data[3][2] = tz; // Translate in z direction
    return result;
}

sa_mat4_t sa_Mat4_Model_Matrix(sa_vec3_t position, sa_vec3_t rotation, sa_vec3_t scale) {
    sa_mat4_t scaleMat = sa_Mat4_Scale(scale.m_x, scale.m_y, scale.m_z);

    sa_mat4_t rotationX =
        sa_Mat4_Rotate_Z(sa_Mat4_Identity(), rotation.m_x); // Rotate around X-axis
    sa_mat4_t rotationY =
        sa_Mat4_Rotate_Y(sa_Mat4_Identity(), rotation.m_y); // Rotate around Y-axis
    sa_mat4_t rotationZ =
        sa_Mat4_Rotate_Z(sa_Mat4_Identity(), rotation.m_z); // Rotate around Z-axis

    sa_mat4_t rotationMat = sa_Mat4_Multiply(rotationZ, sa_Mat4_Multiply(rotationY, rotationX));

    sa_mat4_t translationMat = sa_Mat4_Translate(position.m_x, position.m_y, position.m_z);

    sa_mat4_t modelMatrix =
        sa_Mat4_Multiply(translationMat, sa_Mat4_Multiply(rotationMat, scaleMat));

    return modelMatrix;
}
