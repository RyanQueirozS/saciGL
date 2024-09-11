#include "helpers.h"

vec2 Vec2(float x, float y) {
    vec2 vec;
    vec.x = x;
    vec.y = y;
    return vec;
}

vec3 Vec3(float x, float y, float z) {
    vec3 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}

vec4 Vec4(float x, float y, float z, float w) {
    vec4 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    vec.w = w;
    return vec;
}

color Color(float r, float g, float b, float a) {
    color color_;
    color_.r = r;
    color_.g = g;
    color_.b = b;
    color_.a = a;
    return color_;
}
