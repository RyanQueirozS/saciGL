#include "saci-utils/su-math.h"
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

void sa_Math_Init() {
    __sa_math_preferences.sqrt_function = __sa_default_sqrt; // defaults the sqrt
                                                             // operation to C's math.h
                                                             // sqrt function'

    __sa_math_preferences.tan_function = __sa_default_tan;
}

//------------------------------------------------------------------------------
// Vec3
//------------------------------------------------------------------------------

sa_Vec3_t sa_Vec3_Subtract(sa_Vec3_t a, sa_Vec3_t b) {
    sa_Vec3_t result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

sa_Vec3_t sa_Vec3_Add(sa_Vec3_t a, sa_Vec3_t b) {
    sa_Vec3_t result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

sa_Vec3_t sa_Vec3_Scale(sa_Vec3_t v, float scalar) {
    sa_Vec3_t result = {v.x * scalar, v.y *= scalar, v.z *= scalar};
    return result;
}

sa_Vec3_t sa_Vec3_Normalize(sa_Vec3_t v) {
    float mag = __sa_math_preferences.sqrt_function(v.x * v.x + v.y * v.y + v.z * v.z);
    if (mag == 0.0f) {
        return (sa_Vec3_t){0.0f, 0.0f, 0.0f};
    }
    sa_Vec3_t result = {v.x / mag, v.y / mag, v.z / mag};
    return result;
}

sa_Vec3_t sa_Vec3_Cross(sa_Vec3_t a, sa_Vec3_t b) {
    sa_Vec3_t result = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    return result;
}

float sa_Vec3_Dot(sa_Vec3_t a, sa_Vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

//------------------------------------------------------------------------------
// Color
//------------------------------------------------------------------------------

sa_Color_t sa_Color_From_Hex(sa_u32_t hex) {
    sa_Color_t color;
    color.r =
        ((hex >> 24) & 0xFF) * sa_COLOR_8BIT_INVERSE_MAX; // Extract and convert red component
    color.g =
        ((hex >> 16) & 0xFF) * sa_COLOR_8BIT_INVERSE_MAX; // Extract and convert green component
    color.b =
        ((hex >> 8) & 0xFF) * sa_COLOR_8BIT_INVERSE_MAX; // Extract and convert blue component
    color.a = (hex & 0xFF) * sa_COLOR_8BIT_INVERSE_MAX;  // Extract and convert alpha component
    return color;
}

sa_Color_t sa_Color_From_U8(sa_u8_t r, sa_u8_t g, sa_u8_t b, sa_u8_t a) {
    sa_Color_t color;
    color.r = r * sa_COLOR_8BIT_INVERSE_MAX;
    color.g = g * sa_COLOR_8BIT_INVERSE_MAX;
    color.b = b * sa_COLOR_8BIT_INVERSE_MAX;
    color.a = a * sa_COLOR_8BIT_INVERSE_MAX;
    return color;
}

sa_u32_t sa_Color_To_Hex(sa_Color_t color) {
    sa_u32_t hex = ((sa_u32_t)(color.r * sa_COLOR_8BIT_MAX) << 24) |
                   ((sa_u32_t)(color.g * sa_COLOR_8BIT_MAX) << 16) |
                   ((sa_u32_t)(color.b * sa_COLOR_8BIT_MAX) << 8) |
                   (sa_u32_t)(color.a * sa_COLOR_8BIT_MAX);
    return hex;
}
//------------------------------------------------------------------------------
// Mat4
//------------------------------------------------------------------------------

sa_Mat4_t sa_Mat4_Multiply(sa_Mat4_t a, sa_Mat4_t b) {
    sa_Mat4_t result;
    memset(result.m, 0, sizeof(result.m));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return result;
}

sa_Mat4_t sa_Mat4_Identity() {
    sa_Mat4_t result = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
    return result;
}

sa_Mat4_t sa_Mat4_Look_At(sa_Vec3_t eye, sa_Vec3_t center, sa_Vec3_t up) {
    sa_Vec3_t f = sa_Vec3_Normalize(sa_Vec3_Subtract(center, eye));
    sa_Vec3_t s = sa_Vec3_Normalize(sa_Vec3_Cross(f, up));
    sa_Vec3_t u = sa_Vec3_Cross(s, f);

    sa_Mat4_t result = sa_Mat4_Identity();
    result.m[0][0] = s.x;
    result.m[0][1] = u.x;
    result.m[0][2] = -f.x;
    result.m[1][0] = s.y;
    result.m[1][1] = u.y;
    result.m[1][2] = -f.y;
    result.m[2][0] = s.z;
    result.m[2][1] = u.z;
    result.m[2][2] = -f.z;
    result.m[3][0] = -sa_Vec3_Dot(s, eye);
    result.m[3][1] = -sa_Vec3_Dot(u, eye);
    result.m[3][2] = sa_Vec3_Dot(f, eye);

    return result;
}

sa_Mat4_t sa_Mat4_Perspective(float fov, float aspect, float near, float far) {
    sa_Mat4_t result = {0};
    float tanHalfFov = __sa_math_preferences.tan_function(sa_DEG2RAD_m(fov) / 2.0f);

    result.m[0][0] = 1.0f / (aspect * tanHalfFov);
    result.m[1][1] = 1.0f / tanHalfFov;
    result.m[2][2] = -(far + near) / (far - near);
    result.m[2][3] = -1.0f;
    result.m[3][2] = -(2.0f * far * near) / (far - near);

    return result;
}

sa_Mat4_t sa_Mat4_Ortho(float left, float right, float bottom, float top, float near, float far) {
    sa_Mat4_t result = {0};

    result.m[0][0] = 2.0f / (right - left);
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[2][2] = -2.0f / (far - near);
    result.m[3][0] = -(right + left) / (right - left);
    result.m[3][1] = -(top + bottom) / (top - bottom);
    result.m[3][2] = -(far + near) / (far - near);
    result.m[3][3] = 1.0f;

    return result;
}

sa_Mat4_t sa_Mat4_Rotate_X(sa_Mat4_t mat, float angle) {
    sa_Mat4_t rotation = sa_Mat4_Identity();
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.m[1][1] = cosA;
    rotation.m[1][2] = -sinA;
    rotation.m[2][1] = sinA;
    rotation.m[2][2] = cosA;

    return sa_Mat4_Multiply(mat, rotation);
}

sa_Mat4_t sa_Mat4_Rotate_Y(sa_Mat4_t mat, float angle) {
    sa_Mat4_t rotation = sa_Mat4_Identity();
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.m[0][0] = cosA;
    rotation.m[0][2] = sinA;
    rotation.m[2][0] = -sinA;
    rotation.m[2][2] = cosA;

    return sa_Mat4_Multiply(mat, rotation);
}

sa_Mat4_t sa_RotateMat4_Z(sa_Mat4_t mat, float angle) {
    sa_Mat4_t rotation = sa_Mat4_Identity();
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.m[0][0] = cosA;
    rotation.m[0][1] = -sinA;
    rotation.m[1][0] = sinA;
    rotation.m[1][1] = cosA;

    return sa_Mat4_Multiply(mat, rotation);
}

sa_Mat4_t sa_Mat4_Scale(float sx, float sy, float sz) {
    sa_Mat4_t result = sa_Mat4_Identity();
    result.m[0][0] = sx; // Scale in x direction
    result.m[1][1] = sy; // Scale in y direction
    result.m[2][2] = sz; // Scale in z direction
    return result;
}

sa_Mat4_t sa_Mat4_Translate(float tx, float ty, float tz) {
    sa_Mat4_t result = sa_Mat4_Identity();
    result.m[3][0] = tx; // Translate in x direction
    result.m[3][1] = ty; // Translate in y direction
    result.m[3][2] = tz; // Translate in z direction
    return result;
}

sa_Mat4_t sa_Mat4_Model_Matrix(sa_Vec3_t position, sa_Vec3_t rotation, sa_Vec3_t scale) {
    sa_Mat4_t scaleMat = sa_Mat4_Scale(scale.x, scale.y, scale.z);

    sa_Mat4_t rotationX =
        sa_Mat4_Rotate_X(sa_Mat4_Identity(), rotation.x); // Rotate around X-axis
    sa_Mat4_t rotationY =
        sa_Mat4_Rotate_Y(sa_Mat4_Identity(), rotation.y); // Rotate around Y-axis
    sa_Mat4_t rotationZ =
        sa_RotateMat4_Z(sa_Mat4_Identity(), rotation.z); // Rotate around Z-axis

    sa_Mat4_t rotationMat = sa_Mat4_Multiply(rotationZ, sa_Mat4_Multiply(rotationY, rotationX));

    sa_Mat4_t translationMat = sa_Mat4_Translate(position.x, position.y, position.z);

    sa_Mat4_t modelMatrix =
        sa_Mat4_Multiply(translationMat, sa_Mat4_Multiply(rotationMat, scaleMat));

    return modelMatrix;
}
