#include "saci_util/math.h"

#include "saci_util/internal/general.h"

SaciVec3 saci_vec3_subtract(SaciVec3 a, SaciVec3 b)
{
    SaciVec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}

SaciVec3 saci_vec3_add(SaciVec3 a, SaciVec3 b)
{
    SaciVec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}

SaciVec3 saci_vec3_scale(SaciVec3 v, float scalar)
{
    SaciVec3 result = {v.x * scalar, v.y *= scalar, v.z *= scalar};
    return result;
}

SaciVec3 saci_vec3_normalize(SaciVec3 v)
{
    float mag = SACI_CAST_M(float)(saci_math_sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
    if (mag == 0.0f) {
        return (SaciVec3){0.0f, 0.0f, 0.0f};
    }
    SaciVec3 result = {v.x / mag, v.y / mag, v.z / mag};
    return result;
}

SaciVec3 saci_vec3_cross(SaciVec3 a, SaciVec3 b)
{
    SaciVec3 result = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    return result;
}

float saci_vec3_dot(SaciVec3 a, SaciVec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
