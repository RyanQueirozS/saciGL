#include "saci-utils/su-math.h"
#include <math.h>
#include <string.h>

#define SACI_8BIT_COLOR_MAX 255.0f
#define SACI_8BIT_COLOR_INVERSE_MAX \
    1.0f / SACI_8BIT_COLOR_MAX // Used for Color related math

static double saci_default_sqrt(double x) { // wrapps math.h sqrt func
    return sqrt(x);
}

static double saci_default_tan(double x) { // wrapps math.h sqrt func
    return tan(x);
}

static struct {
    double (*sqrt_function)(double);
    double (*tan_function)(double);
} saci_mathPreferences;

//------------------------------------------------------------------------------
// Init
//------------------------------------------------------------------------------

void saci_InitMath() {
    saci_mathPreferences.sqrt_function = saci_default_sqrt; // defaults the sqrt
                                                            // operation to C's math.h
                                                            // sqrt function'

    saci_mathPreferences.tan_function = saci_default_tan;
}

//------------------------------------------------------------------------------
// Vec3
//------------------------------------------------------------------------------

saci_Vec3 saci_SubtractVec3(saci_Vec3 a, saci_Vec3 b) {
    saci_Vec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

saci_Vec3 saci_AddVec3(saci_Vec3 a, saci_Vec3 b) {
    saci_Vec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

saci_Vec3 saci_MultiplyVec3(saci_Vec3 v, float scalar) {
    saci_Vec3 result = {v.x * scalar, v.y *= scalar, v.z *= scalar};
    return result;
}

saci_Vec3 saci_NormalizeVec3(saci_Vec3 v) {
    float mag = saci_mathPreferences.sqrt_function(v.x * v.x + v.y * v.y + v.z * v.z);
    if (mag == 0.0f) {
        return (saci_Vec3){0.0f, 0.0f, 0.0f};
    }
    saci_Vec3 result = {v.x / mag, v.y / mag, v.z / mag};
    return result;
}

saci_Vec3 saci_CrossVec3(saci_Vec3 a, saci_Vec3 b) {
    saci_Vec3 result = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                        a.x * b.y - a.y * b.x};
    return result;
}

float saci_DotVec3(saci_Vec3 a, saci_Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

//------------------------------------------------------------------------------
// Color
//------------------------------------------------------------------------------

saci_Color saci_ColorFromHex(saci_u32 hex) {
    saci_Color color;
    color.r = ((hex >> 24) & 0xFF) *
              SACI_8BIT_COLOR_INVERSE_MAX; // Extract and convert red component
    color.g = ((hex >> 16) & 0xFF) *
              SACI_8BIT_COLOR_INVERSE_MAX; // Extract and convert green component
    color.b = ((hex >> 8) & 0xFF) *
              SACI_8BIT_COLOR_INVERSE_MAX; // Extract and convert blue component
    color.a =
        (hex & 0xFF) * SACI_8BIT_COLOR_INVERSE_MAX; // Extract and convert alpha component
    return color;
}

saci_Color saci_ColorFromU8(saci_u8 r, saci_u8 g, saci_u8 b, saci_u8 a) {
    saci_Color color;
    color.r = r * SACI_8BIT_COLOR_INVERSE_MAX;
    color.g = g * SACI_8BIT_COLOR_INVERSE_MAX;
    color.b = b * SACI_8BIT_COLOR_INVERSE_MAX;
    color.a = a * SACI_8BIT_COLOR_INVERSE_MAX;
    return color;
}

saci_u32 saci_HexFromColor(saci_Color color) {
    saci_u32 hex = ((saci_u32)(color.r * SACI_8BIT_COLOR_MAX) << 24) |
                   ((saci_u32)(color.g * SACI_8BIT_COLOR_MAX) << 16) |
                   ((saci_u32)(color.b * SACI_8BIT_COLOR_MAX) << 8) |
                   (saci_u32)(color.a * SACI_8BIT_COLOR_MAX);
    return hex;
}
//------------------------------------------------------------------------------
// Mat4
//------------------------------------------------------------------------------
saci_Mat4 saci_MultiplyMat4(saci_Mat4 a, saci_Mat4 b) {
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

saci_Mat4 saci_IdentityMat4() {
    saci_Mat4 result = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
    return result;
}

saci_Mat4 saci_LookAtMat4(saci_Vec3 eye, saci_Vec3 center, saci_Vec3 up) {
    saci_Vec3 f = saci_NormalizeVec3(saci_SubtractVec3(center, eye));
    saci_Vec3 s = saci_NormalizeVec3(saci_CrossVec3(f, up));
    saci_Vec3 u = saci_CrossVec3(s, f);

    saci_Mat4 result = saci_IdentityMat4();
    result.m[0][0] = s.x;
    result.m[0][1] = u.x;
    result.m[0][2] = -f.x;
    result.m[1][0] = s.y;
    result.m[1][1] = u.y;
    result.m[1][2] = -f.y;
    result.m[2][0] = s.z;
    result.m[2][1] = u.z;
    result.m[2][2] = -f.z;
    result.m[3][0] = -saci_DotVec3(s, eye);
    result.m[3][1] = -saci_DotVec3(u, eye);
    result.m[3][2] = saci_DotVec3(f, eye);

    return result;
}

saci_Mat4 saci_PerspectiveMat4(float fov, float aspect, float near, float far) {
    saci_Mat4 result = {0};
    float tanHalfFov = saci_mathPreferences.tan_function(SACI_DEG2RAD(fov) / 2.0f);

    result.m[0][0] = 1.0f / (aspect * tanHalfFov);
    result.m[1][1] = 1.0f / tanHalfFov;
    result.m[2][2] = -(far + near) / (far - near);
    result.m[2][3] = -1.0f;
    result.m[3][2] = -(2.0f * far * near) / (far - near);

    return result;
}

saci_Mat4 saci_OrthoMat4(float left, float right, float bottom, float top, float near,
                         float far) {
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
