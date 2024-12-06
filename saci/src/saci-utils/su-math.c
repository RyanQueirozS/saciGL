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

saci_Vec3 sa_Vec3_Subtract(saci_Vec3 a, saci_Vec3 b) {
    saci_Vec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

saci_Vec3 sa_Vec3_Add(saci_Vec3 a, saci_Vec3 b) {
    saci_Vec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

saci_Vec3 sa_Vec3_Scale(saci_Vec3 v, float scalar) {
    saci_Vec3 result = {v.x * scalar, v.y *= scalar, v.z *= scalar};
    return result;
}

saci_Vec3 sa_Vec3_Normalize(saci_Vec3 v) {
    float mag = __sa_math_preferences.sqrt_function(v.x * v.x + v.y * v.y + v.z * v.z);
    if (mag == 0.0f) {
        return (saci_Vec3){0.0f, 0.0f, 0.0f};
    }
    saci_Vec3 result = {v.x / mag, v.y / mag, v.z / mag};
    return result;
}

saci_Vec3 sa_Vec3_Cross(saci_Vec3 a, saci_Vec3 b) {
    saci_Vec3 result = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    return result;
}

float sa_Vec3_Dot(saci_Vec3 a, saci_Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

//------------------------------------------------------------------------------
// Color
//------------------------------------------------------------------------------

saci_Color sa_Color_From_Hex(saci_u32 hex) {
    saci_Color color;
    color.r =
        ((hex >> 24) & 0xFF) * sa_COLOR_8BIT_INVERSE_MAX; // Extract and convert red component
    color.g =
        ((hex >> 16) & 0xFF) * sa_COLOR_8BIT_INVERSE_MAX; // Extract and convert green component
    color.b =
        ((hex >> 8) & 0xFF) * sa_COLOR_8BIT_INVERSE_MAX; // Extract and convert blue component
    color.a = (hex & 0xFF) * sa_COLOR_8BIT_INVERSE_MAX;  // Extract and convert alpha component
    return color;
}

saci_Color sa_Color_From_U8(saci_u8 r, saci_u8 g, saci_u8 b, saci_u8 a) {
    saci_Color color;
    color.r = r * sa_COLOR_8BIT_INVERSE_MAX;
    color.g = g * sa_COLOR_8BIT_INVERSE_MAX;
    color.b = b * sa_COLOR_8BIT_INVERSE_MAX;
    color.a = a * sa_COLOR_8BIT_INVERSE_MAX;
    return color;
}

saci_u32 sa_Color_To_Hex(saci_Color color) {
    saci_u32 hex = ((saci_u32)(color.r * sa_COLOR_8BIT_MAX) << 24) |
                   ((saci_u32)(color.g * sa_COLOR_8BIT_MAX) << 16) |
                   ((saci_u32)(color.b * sa_COLOR_8BIT_MAX) << 8) |
                   (saci_u32)(color.a * sa_COLOR_8BIT_MAX);
    return hex;
}
//------------------------------------------------------------------------------
// Mat4
//------------------------------------------------------------------------------

saci_Mat4 sa_Mat4_Multiply(saci_Mat4 a, saci_Mat4 b) {
    saci_Mat4 result;
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

saci_Mat4 sa_Mat4_Identity() {
    saci_Mat4 result = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
    return result;
}

saci_Mat4 sa_Mat4_Look_At(saci_Vec3 eye, saci_Vec3 center, saci_Vec3 up) {
    saci_Vec3 f = sa_Vec3_Normalize(sa_Vec3_Subtract(center, eye));
    saci_Vec3 s = sa_Vec3_Normalize(sa_Vec3_Cross(f, up));
    saci_Vec3 u = sa_Vec3_Cross(s, f);

    saci_Mat4 result = sa_Mat4_Identity();
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

saci_Mat4 sa_Mat4_Perspective(float fov, float aspect, float near, float far) {
    saci_Mat4 result = {0};
    float tanHalfFov = __sa_math_preferences.tan_function(sa_DEG2RAD_m(fov) / 2.0f);

    result.m[0][0] = 1.0f / (aspect * tanHalfFov);
    result.m[1][1] = 1.0f / tanHalfFov;
    result.m[2][2] = -(far + near) / (far - near);
    result.m[2][3] = -1.0f;
    result.m[3][2] = -(2.0f * far * near) / (far - near);

    return result;
}

saci_Mat4 sa_Mat4_Ortho(float left, float right, float bottom, float top, float near, float far) {
    saci_Mat4 result = {0};

    result.m[0][0] = 2.0f / (right - left);
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[2][2] = -2.0f / (far - near);
    result.m[3][0] = -(right + left) / (right - left);
    result.m[3][1] = -(top + bottom) / (top - bottom);
    result.m[3][2] = -(far + near) / (far - near);
    result.m[3][3] = 1.0f;

    return result;
}

saci_Mat4 sa_Mat4_Rotate_X(saci_Mat4 mat, float angle) {
    saci_Mat4 rotation = sa_Mat4_Identity();
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.m[1][1] = cosA;
    rotation.m[1][2] = -sinA;
    rotation.m[2][1] = sinA;
    rotation.m[2][2] = cosA;

    return sa_Mat4_Multiply(mat, rotation);
}

saci_Mat4 sa_Mat_Rotate_Y(saci_Mat4 mat, float angle) {
    saci_Mat4 rotation = sa_Mat4_Identity();
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.m[0][0] = cosA;
    rotation.m[0][2] = sinA;
    rotation.m[2][0] = -sinA;
    rotation.m[2][2] = cosA;

    return sa_Mat4_Multiply(mat, rotation);
}

saci_Mat4 sa_RotateMat4_Z(saci_Mat4 mat, float angle) {
    saci_Mat4 rotation = sa_Mat4_Identity();
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.m[0][0] = cosA;
    rotation.m[0][1] = -sinA;
    rotation.m[1][0] = sinA;
    rotation.m[1][1] = cosA;

    return sa_Mat4_Multiply(mat, rotation);
}

saci_Mat4 sa_Mat4_Scale(float sx, float sy, float sz) {
    saci_Mat4 result = sa_Mat4_Identity();
    result.m[0][0] = sx; // Scale in x direction
    result.m[1][1] = sy; // Scale in y direction
    result.m[2][2] = sz; // Scale in z direction
    return result;
}

saci_Mat4 sa_Mat4_Translate(float tx, float ty, float tz) {
    saci_Mat4 result = sa_Mat4_Identity();
    result.m[3][0] = tx; // Translate in x direction
    result.m[3][1] = ty; // Translate in y direction
    result.m[3][2] = tz; // Translate in z direction
    return result;
}

saci_Mat4 sa_Mat4_Model_Matrix(saci_Vec3 position, saci_Vec3 rotation, saci_Vec3 scale) {
    saci_Mat4 scaleMat = sa_Mat4_Scale(scale.x, scale.y, scale.z);

    saci_Mat4 rotationX =
        sa_Mat4_Rotate_X(sa_Mat4_Identity(), rotation.x); // Rotate around X-axis
    saci_Mat4 rotationY =
        sa_Mat_Rotate_Y(sa_Mat4_Identity(), rotation.y); // Rotate around Y-axis
    saci_Mat4 rotationZ =
        sa_RotateMat4_Z(sa_Mat4_Identity(), rotation.z); // Rotate around Z-axis

    saci_Mat4 rotationMat = sa_Mat4_Multiply(rotationZ, sa_Mat4_Multiply(rotationY, rotationX));

    saci_Mat4 translationMat = sa_Mat4_Translate(position.x, position.y, position.z);

    saci_Mat4 modelMatrix =
        sa_Mat4_Multiply(translationMat, sa_Mat4_Multiply(rotationMat, scaleMat));

    return modelMatrix;
}
