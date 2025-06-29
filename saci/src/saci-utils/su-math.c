#include "saci-utils/su-math.h"
#include "saci-utils/su-general.h"

#include <math.h>
#include <string.h>

#define su_COLOR_8BIT_MAX (255.0f)
#define su_COLOR_8BIT_INVERSE_MAX (1.0f / su_COLOR_8BIT_MAX) // Used for Color related math

static double su_default_sqrt_s(double x) { // wrapps math.h sqrt func
    return sqrt(x);
}

static double su_default_tan_s(double x) { // wrapps math.h tan func
    return tan(x);
}

static struct {
    double (*sqrt_function)(double);
    double (*tan_function)(double);
} su_math_preferences = {
    .sqrt_function = su_default_sqrt_s,
    .tan_function = su_default_tan_s,
};

//------------------------------------------------------------------------------
// Vec3
//------------------------------------------------------------------------------

su_vec3 su_Vec3_Subtract(su_vec3 a, su_vec3 b) {
    su_vec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

su_vec3 su_Vec3_Add(su_vec3 a, su_vec3 b) {
    su_vec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

su_vec3 su_Vec3_Scale(su_vec3 v, float scalar) {
    su_vec3 result = {v.x * scalar, v.y *= scalar, v.z *= scalar};
    return result;
}

su_vec3 su_Vec3_Normalize(su_vec3 v) {
    float mag = su_Scast_To_m(float)(su_math_preferences.sqrt_function(v.x * v.x + v.y * v.y + v.z * v.z));
    if (mag == 0.0f) {
        return (su_vec3){0.0f, 0.0f, 0.0f};
    }
    su_vec3 result = {v.x / mag, v.y / mag, v.z / mag};
    return result;
}

su_vec3 su_Vec3_Cross(su_vec3 a, su_vec3 b) {
    su_vec3 result = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    return result;
}

float su_Vec3_Dot(su_vec3 a, su_vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

//------------------------------------------------------------------------------
// Color
//------------------------------------------------------------------------------

su_color su_Color_From_Hex(su_u32 hex) {
    su_color color;
    color.r = su_Scast_To_m(float)(((hex >> 24)) & 0xFF) * su_COLOR_8BIT_INVERSE_MAX;
    color.g = su_Scast_To_m(float)(((hex >> 16)) & 0xFF) * su_COLOR_8BIT_INVERSE_MAX;
    color.b = su_Scast_To_m(float)(((hex >> 8)) & 0xFF) * su_COLOR_8BIT_INVERSE_MAX;
    color.a = su_Scast_To_m(float)((hex & 0xFF)) * su_COLOR_8BIT_INVERSE_MAX;
    return color;
}

su_color su_Color_From_U8(su_u8 r, su_u8 g, su_u8 b, su_u8 a) {
    su_color color;
    color.r = su_Scast_To_m(float)(r) * su_COLOR_8BIT_INVERSE_MAX;
    color.g = su_Scast_To_m(float)(g) * su_COLOR_8BIT_INVERSE_MAX;
    color.b = su_Scast_To_m(float)(b) * su_COLOR_8BIT_INVERSE_MAX;
    color.a = su_Scast_To_m(float)(a) * su_COLOR_8BIT_INVERSE_MAX;
    return color;
}

su_u32 su_Color_To_Hex(su_color color) {
    su_u32 hex = ((su_u32)(color.r * su_COLOR_8BIT_MAX) << 24) |
                 ((su_u32)(color.g * su_COLOR_8BIT_MAX) << 16) |
                 ((su_u32)(color.b * su_COLOR_8BIT_MAX) << 8) |
                 (su_u32)(color.a * su_COLOR_8BIT_MAX);
    return hex;
}
//------------------------------------------------------------------------------
// Mat4
//------------------------------------------------------------------------------

su_mat4 su_Mat4_Multiply(su_mat4 a, su_mat4 b) {
    su_mat4 result;
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

su_mat4 su_Mat4_Look_At(su_vec3 eye, su_vec3 center, su_vec3 up) {
    su_vec3 f = su_Vec3_Normalize(su_Vec3_Subtract(center, eye));
    su_vec3 s = su_Vec3_Normalize(su_Vec3_Cross(f, up));
    su_vec3 u = su_Vec3_Cross(s, f);

    su_mat4 result = SU_IDENTITY_MAT4;
    result.data[0][0] = s.x;
    result.data[0][1] = u.x;
    result.data[0][2] = -f.x;
    result.data[1][0] = s.y;
    result.data[1][1] = u.y;
    result.data[1][2] = -f.y;
    result.data[2][0] = s.z;
    result.data[2][1] = u.z;
    result.data[2][2] = -f.z;
    result.data[3][0] = -su_Vec3_Dot(s, eye);
    result.data[3][1] = -su_Vec3_Dot(u, eye);
    result.data[3][2] = su_Vec3_Dot(f, eye);

    return result;
}

// TODO evaluate if there is a way not to use tan operations
su_mat4 su_Mat4_Perspective(float fov, float aspect, float near, float far) {
    su_mat4 result = {0};
    float tanHalfFov = su_Scast_To_m(float)(su_math_preferences.tan_function(su_Deg_To_Rad_m(fov) / 2.0f));

    result.data[0][0] = 1.0f / (aspect * tanHalfFov);
    result.data[1][1] = 1.0f / tanHalfFov;
    result.data[2][2] = -(far + near) / (far - near);
    result.data[2][3] = -1.0f;
    result.data[3][2] = -(2.0f * far * near) / (far - near);

    return result;
}

su_mat4 su_Mat4_Ortho(float left, float right, float bottom, float top, float near, float far) {
    su_mat4 result = {0};

    result.data[0][0] = 2.0f / (right - left);
    result.data[1][1] = 2.0f / (top - bottom);
    result.data[2][2] = -2.0f / (far - near);
    result.data[3][0] = -(right + left) / (right - left);
    result.data[3][1] = -(top + bottom) / (top - bottom);
    result.data[3][2] = -(far + near) / (far - near);
    result.data[3][3] = 1.0f;

    return result;
}

su_mat4 su_Mat4_Model_Matrix_RTS(su_vec3 position, su_vec3 rotation, su_vec3 scale) {
    su_mat4 scaleMat = su_Mat4_Scale(scale.x, scale.y, scale.z);

    su_mat4 rotationX =
        su_Mat4_Rotate_Z(SU_IDENTITY_MAT4, rotation.x); // Rotate around X-axis
    su_mat4 rotationY =
        su_Mat4_Rotate_Y(SU_IDENTITY_MAT4, rotation.y); // Rotate around Y-axis
    su_mat4 rotationZ =
        su_Mat4_Rotate_Z(SU_IDENTITY_MAT4, rotation.z); // Rotate around Z-axis

    su_mat4 rotationMat = su_Mat4_Multiply(rotationZ, su_Mat4_Multiply(rotationY, rotationX));

    su_mat4 translationMat = su_Mat4_Translate(position.x, position.y, position.z);

    su_mat4 modelMatrix = su_Mat4_Multiply(translationMat, su_Mat4_Multiply(rotationMat, scaleMat));

    return modelMatrix;
}

su_mat4 su_Mat4_Model_Matrix_TRS(su_vec3 position, su_vec3 rotation, su_vec3 scale) {
    su_mat4 scaleMat = su_Mat4_Scale(scale.x, scale.y, scale.z);

    su_mat4 rotationX = su_Mat4_Rotate_X(SU_IDENTITY_MAT4, rotation.x);
    su_mat4 rotationY = su_Mat4_Rotate_Y(SU_IDENTITY_MAT4, rotation.y);
    su_mat4 rotationZ = su_Mat4_Rotate_Z(SU_IDENTITY_MAT4, rotation.z);

    su_mat4 rotationMat = su_Mat4_Multiply(rotationZ, su_Mat4_Multiply(rotationY, rotationX));

    su_mat4 translationMat = su_Mat4_Translate(position.x, position.y, position.z);

    // Rotate first -> scale -> then translate
    su_mat4 modelMatrix = su_Mat4_Multiply(rotationMat, su_Mat4_Multiply(translationMat, scaleMat));
    return modelMatrix;
}

su_mat4 su_Mat4_Rotate_X(su_mat4 mat, float angle) {
    su_mat4 rotation = SU_IDENTITY_MAT4;
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.data[1][1] = cosA;
    rotation.data[1][2] = -sinA;
    rotation.data[2][1] = sinA;
    rotation.data[2][2] = cosA;

    return su_Mat4_Multiply(mat, rotation);
}

su_mat4 su_Mat4_Rotate_Y(su_mat4 mat, float angle) {
    su_mat4 rotation = SU_IDENTITY_MAT4;
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.data[0][0] = cosA;
    rotation.data[0][2] = sinA;
    rotation.data[2][0] = -sinA;
    rotation.data[2][2] = cosA;

    return su_Mat4_Multiply(mat, rotation);
}

su_mat4 su_Mat4_Rotate_Z(su_mat4 mat, float angle) {
    su_mat4 rotation = SU_IDENTITY_MAT4;
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    rotation.data[0][0] = cosA;
    rotation.data[0][1] = -sinA;
    rotation.data[1][0] = sinA;
    rotation.data[1][1] = cosA;

    return su_Mat4_Multiply(mat, rotation);
}

su_mat4 su_Mat4_Scale(float sx, float sy, float sz) {
    su_mat4 result = SU_IDENTITY_MAT4;
    result.data[0][0] = sx; // Scale in x direction
    result.data[1][1] = sy; // Scale in y direction
    result.data[2][2] = sz; // Scale in z direction
    return result;
}

su_mat4 su_Mat4_Translate(float tx, float ty, float tz) {
    su_mat4 result = SU_IDENTITY_MAT4;
    result.data[3][0] = tx; // Translate in x direction
    result.data[3][1] = ty; // Translate in y direction
    result.data[3][2] = tz; // Translate in z direction
    return result;
}
