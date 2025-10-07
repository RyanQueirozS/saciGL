#include "saci-utils/math/su-math-vec.h"

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
    float mag = su_CAST_M(float)(su_math_sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
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
