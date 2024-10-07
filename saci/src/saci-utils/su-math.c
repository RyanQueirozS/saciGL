#include "saci-utils/su-math.h"
#include <math.h>
#include <string.h>

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

void saci_InitMath() {
    saci_mathPreferences.sqrt_function = saci_default_sqrt; // defaults the sqrt
                                                            // operation to C's math.h
                                                            // sqrt function'

    saci_mathPreferences.tan_function = saci_default_tan;
}

saci_Vec3 saci_SubtractVec3(saci_Vec3 a, saci_Vec3 b) {
    saci_Vec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

saci_Vec3 saci_NormalizeVec3(saci_Vec3 v) {
    float mag = saci_mathPreferences.sqrt_function(v.x * v.x + v.y * v.y + v.z * v.z);
    saci_Vec3 result = {v.x / mag, v.y / mag, v.z / mag};
    return result;
}

saci_Vec3 saci_CrossVec3(saci_Vec3 a, saci_Vec3 b) {
    saci_Vec3 result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
    return result;
}

float saci_DotVec3(saci_Vec3 a, saci_Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

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

saci_Mat4 saci_OrthoMat4(float left, float right, float bottom, float top, float near, float far) {
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
