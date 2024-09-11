#ifndef __SACI_HELPERS_H__
#define __SACI_HELPERS_H__

#include <stdint.h>

#define ARRLEN(array) (sizeof(array) / sizeof(array[0]))

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef struct vec2 {
    float x, y;
} vec2;
vec2 Vec2(float x, float y);

typedef struct vec3 {
    float x, y, z;
} vec3;
vec3 Vec3(float x, float y, float z);

typedef struct vec4 {
    float x, y, z, w;
} vec4;
vec4 Vec4(float x, float y, float z, float w);

typedef struct color {
    float r, g, b, a;
} color;
color Color(float r, float g, float b, float a);

#endif
